
#pragma options align=mac68k

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <dos.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>

#include <exec/exec.h>
#include <devices/serial.h>
#include <proto/exec.h>
#include <clib/socket_protos.h>

#ifdef AMIPX
// These include file are distributed with amipx.library
#include <amipx.h>
#include <amipx_protos.h>
#include <amipx_ppc_pragmas.h>
#endif

#pragma options align=power

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"
#include "m_swap.h"

#include "doomstat.h"

#include "i_net.h"

extern struct ExecBase *SysBase;

#define	BeginIO(ioRequest)	_BeginIO(ioRequest)

extern void _BeginIO(struct IORequest *ioRequest);

extern void ppctimer (unsigned int *time);
extern int bus_clock;

//
// NETWORKING
//

/**********************************************************************/
/**********************************************************************/
/* TCP/IP stuff */

struct Library *SocketBase = NULL;

static int IP_DOOMPORT = (IPPORT_USERRESERVED + 0x1d);

static int IP_sendsocket = -1;
static int IP_insocket = -1;

static struct sockaddr_in IP_sendaddress[MAXNETNODES];

static void (*netget) (void);
static void (*netsend) (void);


/**********************************************************************/
//
// IP_UDPsocket
//
static int IP_UDPsocket (void)
{
  int s;

  // allocate a socket
  s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s < 0)
    I_Error ("can't create socket: %s", strerror(errno));
  return s;
}

/**********************************************************************/
//
// IP_BindToLocalPort
//
static void IP_BindToLocalPort (int s, int port)
{
  int v;
  struct sockaddr_in address;

  memset (&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = port;

  v = bind (s, (void *)&address, sizeof(address));
  if (v == -1)
    I_Error ("BindToPort: bind: %s", strerror(errno));
}


/**********************************************************************/
//
// IP_PacketSend
//
static void IP_PacketSend (void)
{
  int  c;
  doomdata_t sw;

  // byte swap
  sw.checksum = htonl(netbuffer->checksum);
  sw.player = netbuffer->player;
  sw.retransmitfrom = netbuffer->retransmitfrom;
  sw.starttic = netbuffer->starttic;
  sw.numtics = netbuffer->numtics;
  for (c = 0 ; c < netbuffer->numtics; c++) {
    sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
    sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
    sw.cmds[c].angleturn = htons(netbuffer->cmds[c].angleturn);
    sw.cmds[c].consistancy = htons(netbuffer->cmds[c].consistancy);
    sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
    sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
  }

  //printf ("sending %i\n",gametic);
  c = sendto (IP_sendsocket , (UBYTE *)&sw, doomcom->datalength,
              0, (void *)&IP_sendaddress[doomcom->remotenode],
              sizeof(IP_sendaddress[doomcom->remotenode]));
//  if (c == -1)
//    /* why does AmiTCP 4.3 return EINVAL or ENOENT instead of EWOULDBLOCK ??? */
//    if (errno != EWOULDBLOCK)
//      I_Error ("SendPacket error: %s",strerror(errno));
}


/**********************************************************************/
//
// IP_PacketGet
//
static void IP_PacketGet (void)
{
  int i, c;
  struct sockaddr_in fromaddress;
  LONG fromlen;
  doomdata_t sw;

  fromlen = sizeof(fromaddress);
  c = recvfrom (IP_insocket, (UBYTE *)&sw, sizeof(sw), 0,
                (struct sockaddr *)&fromaddress, &fromlen);
  if (c == -1) {
    /* why does AmiTCP 4.3 return EINVAL or ENOENT instead of EWOULDBLOCK ??? */
//    if (errno != EWOULDBLOCK)
//      I_Error ("GetPacket: %s",strerror(errno));
    doomcom->remotenode = -1;  // no packet
    return;
  }

  {
    static int first=1;
    if (first)
      printf("len=%d:p=[0x%x 0x%x] \n", c, *(int*)&sw, *((int*)&sw+1));
    first = 0;
  }

  // find remote node number
  for (i = 0; i < doomcom->numnodes; i++)
    if (fromaddress.sin_addr.s_addr == IP_sendaddress[i].sin_addr.s_addr)
      break;

  if (i == doomcom->numnodes) {
    // packet is not from one of the players (new game broadcast)
    doomcom->remotenode = -1;  // no packet
    return;
  }

  doomcom->remotenode = i;   // good packet from a game player
  doomcom->datalength = c;

  // byte swap
  netbuffer->checksum = ntohl(sw.checksum);
  netbuffer->player = sw.player;
  netbuffer->retransmitfrom = sw.retransmitfrom;
  netbuffer->starttic = sw.starttic;
  netbuffer->numtics = sw.numtics;

  for (c = 0; c < netbuffer->numtics; c++) {
    netbuffer->cmds[c].forwardmove = sw.cmds[c].forwardmove;
    netbuffer->cmds[c].sidemove = sw.cmds[c].sidemove;
    netbuffer->cmds[c].angleturn = ntohs(sw.cmds[c].angleturn);
    netbuffer->cmds[c].consistancy = ntohs(sw.cmds[c].consistancy);
    netbuffer->cmds[c].chatchar = sw.cmds[c].chatchar;
    netbuffer->cmds[c].buttons = sw.cmds[c].buttons;
  }
}


/**********************************************************************/
#if 0
static int IP_GetLocalAddress (void)
{
  char hostname[1024];
  struct hostent* hostentry; // host information entry
  int v;

  // get local address
  v = gethostname (hostname, sizeof(hostname));
  if (v == -1)
    I_Error ("IP_GetLocalAddress : gethostname: errno %d",errno);

  hostentry = gethostbyname (hostname);
  if (!hostentry)
    I_Error ("IP_GetLocalAddress : gethostbyname: couldn't get local host");

  return *(int *)hostentry->h_addr_list[0];
}
#endif

/**********************************************************************/
//
// IP_InitNetwork
//
static void IP_InitNetwork (int i)
{
  char trueval = true;
  struct hostent* hostentry; // host information entry

  if ((SocketBase = OpenLibrary ("bsdsocket.library", 0)) == NULL)
    I_Error ("OpenLibrary(\"bsdsocket.library\") failed");

  netsend = IP_PacketSend;
  netget = IP_PacketGet;
  netgame = true;

  // parse player number and host list
  doomcom->consoleplayer = myargv[i+1][0]-'1';

  doomcom->numnodes = 1; // this node for sure

  i++;
  while (++i < myargc && myargv[i][0] != '-') {
    IP_sendaddress[doomcom->numnodes].sin_family = AF_INET;
    IP_sendaddress[doomcom->numnodes].sin_port = htons(IP_DOOMPORT);
    if (myargv[i][0] == '.') {
      IP_sendaddress[doomcom->numnodes].sin_addr.s_addr = inet_addr (myargv[i]+1);
    } else {
      hostentry = gethostbyname (myargv[i]);
      if (!hostentry)
        I_Error ("gethostbyname: couldn't find %s", myargv[i]);
      IP_sendaddress[doomcom->numnodes].sin_addr.s_addr =
                                              *(int *)hostentry->h_addr_list[0];
    }
    doomcom->numnodes++;
  }

  doomcom->id = DOOMCOM_ID;
  doomcom->numplayers = doomcom->numnodes;

  // build message to receive
  IP_insocket = IP_UDPsocket ();
  IP_sendsocket = IP_UDPsocket ();

  IP_BindToLocalPort (IP_insocket, htons(IP_DOOMPORT));

  /* set both sockets to non-blocking */
  if (IoctlSocket (IP_insocket, FIONBIO, &trueval) == -1 ||
      IoctlSocket (IP_sendsocket, FIONBIO, &trueval) == -1)
    I_Error ("IoctlSocket() failed: %s", strerror(errno));
}

/**********************************************************************/
static void IP_Shutdown (void)
{
  if (IP_insocket != -1) {
    CloseSocket (IP_insocket);
    IP_insocket = -1;
  }
  if (IP_sendsocket != -1) {
    CloseSocket (IP_sendsocket);
    IP_sendsocket = -1;
  }
  if (SocketBase != NULL) {
    CloseLibrary (SocketBase);
    SocketBase = NULL;
  }
}

/**********************************************************************/
/**********************************************************************/
/* experimental low level serial port stuff */

static struct MsgPort *SER_writelen_mp = NULL;
static struct MsgPort *SER_readlen_mp = NULL;
static struct MsgPort *SER_write_mp = NULL;
static struct MsgPort *SER_read_mp = NULL;
static struct IOExtSer *SER_writelen_io = NULL;
static struct IOExtSer *SER_readlen_io = NULL;
static struct IOExtSer *SER_write_io = NULL;
static struct IOExtSer *SER_read_io = NULL;
static BOOL SER_is_open = FALSE;
static BOOL SER_write_in_progress = FALSE;
static BOOL SER_read_in_progress = FALSE;
static BOOL SER_readlen_in_progress = FALSE;
static BOOL SER_read_waiting_for_len;
static UBYTE SER_get_len, SER_get_len2;
static doomdata_t SER_get_sw;

/**********************************************************************/
//
// SER_PacketSend
//
static void SER_PacketSend (void)
{
  int  c;
  static doomdata_t sw;
  static UBYTE len;

  if (SER_write_in_progress) {

//    if (!CheckIO ((struct IORequest *)SER_write_io))
//      return;       /* previous write hasn't finished yet, forget it */

    WaitIO ((struct IORequest *)SER_writelen_io);
    WaitIO ((struct IORequest *)SER_write_io);
    SER_write_in_progress = FALSE;
  }

  // byte swap
  sw.checksum = htonl(netbuffer->checksum);
  sw.player = netbuffer->player;
  sw.retransmitfrom = netbuffer->retransmitfrom;
  sw.starttic = netbuffer->starttic;
  sw.numtics = netbuffer->numtics;
  for (c = 0 ; c < netbuffer->numtics; c++) {
    sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
    sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
    sw.cmds[c].angleturn = htons(netbuffer->cmds[c].angleturn);
    sw.cmds[c].consistancy = htons(netbuffer->cmds[c].consistancy);
    sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
    sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
  }

  len = doomcom->datalength;

  if (len > 0) {

//    printf ("Sending len = %d  %08x %08x ...\n", len, ((ULONG *)&sw)[0],
//            ((ULONG *)&sw)[1]);

    SER_writelen_io->IOSer.io_Length = 1;
    SER_writelen_io->IOSer.io_Data = &len;
    SER_writelen_io->IOSer.io_Command = CMD_WRITE;
    SER_writelen_io->IOSer.io_Flags |= IOF_QUICK;
    BeginIO ((struct IORequest *)SER_writelen_io);

    SER_write_io->IOSer.io_Length = len;
    SER_write_io->IOSer.io_Data = (UBYTE *)&sw;
    SER_write_io->IOSer.io_Command = CMD_WRITE;
    SER_write_io->IOSer.io_Flags |= IOF_QUICK;
    BeginIO ((struct IORequest *)SER_write_io);

    SER_write_in_progress = TRUE;

  }
}

/**********************************************************************/
//
// SER_PacketGet
//
static void SER_PacketGet (void)
{
  int c;

  if (SER_read_waiting_for_len) {

    if (!CheckIO ((struct IORequest *)SER_readlen_io)) {
      doomcom->remotenode = -1;  // no packet
      return;
    }

    WaitIO ((struct IORequest *)SER_readlen_io);/* should return immediately */
    SER_readlen_in_progress = FALSE;

    SER_get_len2 = SER_get_len;

    if (SER_get_len2 != 0) {

//      printf ("Receiving len = %d, lenstatus = %d", (int)SER_get_len2,
//              SER_readlen_io->IOSer.io_Error);

      SER_read_io->IOSer.io_Length = SER_get_len2;
      SER_read_io->IOSer.io_Data = (UBYTE *)&SER_get_sw;
      SER_read_io->IOSer.io_Command = CMD_READ;
      SER_read_io->IOSer.io_Flags |= IOF_QUICK;
      BeginIO ((struct IORequest *)SER_read_io);
      SER_read_in_progress = TRUE;

      SER_read_waiting_for_len = FALSE;

    }
    SER_readlen_io->IOSer.io_Length = 1;
    SER_readlen_io->IOSer.io_Data = &SER_get_len;
    SER_readlen_io->IOSer.io_Command = CMD_READ;
    SER_readlen_io->IOSer.io_Flags |= IOF_QUICK;
    BeginIO ((struct IORequest *)SER_readlen_io);
    SER_readlen_in_progress = TRUE;

    doomcom->remotenode = -1;  // no packet
    return;
  }

  if (SER_read_in_progress)
    if (!CheckIO ((struct IORequest *)SER_read_io)) {
      doomcom->remotenode = -1;  // no packet
      return;
    }

  WaitIO ((struct IORequest *)SER_read_io); /* should return immediately */
  SER_read_in_progress = FALSE;

//  printf (", status = %d  %08x %08x ...\n", SER_read_io->IOSer.io_Error,
//          ((ULONG *)&SER_get_sw)[0], ((ULONG *)&SER_get_sw)[1]);

  {
    static int first=1;
    if (first)
      printf("len=%d:p=[0x%x 0x%x] \n", SER_get_len2, *(int*)&SER_get_sw,
             *((int*)&SER_get_sw+1));
    first = 0;
  }

  doomcom->remotenode = 1;  /* from the other player */
  doomcom->datalength = SER_get_len2;

  // byte swap
  netbuffer->checksum = ntohl(SER_get_sw.checksum);
  netbuffer->player = SER_get_sw.player;
  netbuffer->retransmitfrom = SER_get_sw.retransmitfrom;
  netbuffer->starttic = SER_get_sw.starttic;
  netbuffer->numtics = SER_get_sw.numtics;

  for (c = 0; c < netbuffer->numtics; c++) {
    netbuffer->cmds[c].forwardmove = SER_get_sw.cmds[c].forwardmove;
    netbuffer->cmds[c].sidemove = SER_get_sw.cmds[c].sidemove;
    netbuffer->cmds[c].angleturn = ntohs(SER_get_sw.cmds[c].angleturn);
    netbuffer->cmds[c].consistancy = ntohs(SER_get_sw.cmds[c].consistancy);
    netbuffer->cmds[c].chatchar = SER_get_sw.cmds[c].chatchar;
    netbuffer->cmds[c].buttons = SER_get_sw.cmds[c].buttons;
  }

  SER_read_waiting_for_len = TRUE;
}

/**********************************************************************/
//
// SER_InitNetwork
//
void SER_InitNetwork (int i)
{
  int unit, speed;

  netsend = SER_PacketSend;
  netget = SER_PacketGet;
  netgame = true;

  // parse player number and host list
  doomcom->consoleplayer = myargv[i+1][0]-'1';

  doomcom->numnodes = 2;
  doomcom->id = DOOMCOM_ID;
  doomcom->numplayers = doomcom->numnodes;

  if ((SER_writelen_mp = CreatePort (NULL, 0)) == NULL ||
      (SER_readlen_mp = CreatePort (NULL, 0)) == NULL ||
      (SER_write_mp = CreatePort (NULL, 0)) == NULL ||
      (SER_read_mp = CreatePort (NULL, 0)) == NULL ||
      (SER_writelen_io = (struct IOExtSer *)CreateExtIO (SER_writelen_mp,
                                            sizeof(struct IOExtSer))) == NULL ||
      (SER_readlen_io = (struct IOExtSer *)CreateExtIO (SER_readlen_mp,
                                            sizeof(struct IOExtSer))) == NULL ||
      (SER_write_io = (struct IOExtSer *)CreateExtIO (SER_write_mp,
                                            sizeof(struct IOExtSer))) == NULL ||
      (SER_read_io = (struct IOExtSer *)CreateExtIO (SER_read_mp,
                                            sizeof(struct IOExtSer))) == NULL)
    I_Error ("Can't create port");

  unit = atoi(myargv[i+3]);
  speed = atoi(myargv[i+4]);

  SER_write_io->io_SerFlags = SERF_XDISABLED | SERF_RAD_BOOGIE | SERF_7WIRE;

  if (OpenDevice (myargv[i+2], unit, (struct IORequest *)SER_write_io, 0) != 0)
    I_Error ("Can't open %s port %d", myargv[i+2], unit);
  SER_is_open = TRUE;

  SER_write_io->io_SerFlags &= ~SERF_PARTY_ON;
  SER_write_io->io_SerFlags |= SERF_XDISABLED | SERF_RAD_BOOGIE | SERF_7WIRE;
  SER_write_io->io_Baud = speed;
  SER_write_io->io_ReadLen = 8;
  SER_write_io->io_WriteLen = 8;
  SER_write_io->io_StopBits = 1;
  SER_write_io->io_RBufLen = 65536;
  SER_write_io->IOSer.io_Command = SDCMD_SETPARAMS;
  if (DoIO ((struct IORequest *)SER_write_io) != 0)
    I_Error ("Error setting serial parameters (speed = %d)", speed);

  CopyMem (SER_write_io, SER_read_io, sizeof(struct IOExtSer));
  SER_read_io->IOSer.io_Message.mn_ReplyPort = SER_read_mp;

  CopyMem (SER_write_io, SER_readlen_io, sizeof(struct IOExtSer));
  SER_readlen_io->IOSer.io_Message.mn_ReplyPort = SER_readlen_mp;

  CopyMem (SER_write_io, SER_writelen_io, sizeof(struct IOExtSer));
  SER_writelen_io->IOSer.io_Message.mn_ReplyPort = SER_writelen_mp;

  SER_readlen_io->IOSer.io_Length = 1;
  SER_readlen_io->IOSer.io_Data = &SER_get_len;
  SER_readlen_io->IOSer.io_Command = CMD_READ;
  SER_readlen_io->IOSer.io_Flags |= IOF_QUICK;
  BeginIO ((struct IORequest *)SER_readlen_io);
  SER_readlen_in_progress = TRUE;

  SER_read_waiting_for_len = TRUE;

}

/**********************************************************************/
static void SER_Shutdown (void)
{
  if (SER_is_open) {
    if (SER_readlen_in_progress) {
      AbortIO ((struct IORequest *)SER_readlen_io);
      WaitIO ((struct IORequest *)SER_readlen_io);
      SER_readlen_in_progress = FALSE;
    }
    if (SER_read_in_progress) {
      AbortIO ((struct IORequest *)SER_read_io);
      WaitIO ((struct IORequest *)SER_read_io);
      SER_read_in_progress = FALSE;
    }
    if (SER_write_in_progress) {
      AbortIO ((struct IORequest *)SER_writelen_io);
      WaitIO ((struct IORequest *)SER_writelen_io);
      AbortIO ((struct IORequest *)SER_write_io);
      WaitIO ((struct IORequest *)SER_write_io);
      SER_write_in_progress = FALSE;
    }
    CloseDevice ((struct IORequest *)SER_write_io);
    SER_is_open = FALSE;
  }
  if (SER_readlen_io != NULL) {
    DeleteExtIO ((struct IORequest *)SER_readlen_io);
    SER_readlen_io = NULL;
  }
  if (SER_readlen_mp != NULL) {
    DeletePort (SER_readlen_mp);
    SER_readlen_mp = NULL;
  }
  if (SER_writelen_io != NULL) {
    DeleteExtIO ((struct IORequest *)SER_writelen_io);
    SER_writelen_io = NULL;
  }
  if (SER_writelen_mp != NULL) {
    DeletePort (SER_writelen_mp);
    SER_writelen_mp = NULL;
  }
  if (SER_read_io != NULL) {
    DeleteExtIO ((struct IORequest *)SER_read_io);
    SER_read_io = NULL;
  }
  if (SER_read_mp != NULL) {
    DeletePort (SER_read_mp);
    SER_read_mp = NULL;
  }
  if (SER_write_io != NULL) {
    DeleteExtIO ((struct IORequest *)SER_write_io);
    SER_write_io = NULL;
  }
  if (SER_write_mp != NULL) {
    DeletePort (SER_write_mp);
    SER_write_mp = NULL;
  }
}

/**********************************************************************/
/**********************************************************************/

#ifdef AMIPX

/**********************************************************************/
/* experimental IPX stuff */

#define IPX_NUMPACKETS      10          // max outstanding packets before loss

#pragma options align=mac68k

// setupdata_t is used as doomdata_t during setup
typedef struct
{
  WORD gameid;      // so multiple games can setup at once
  WORD drone;       // You must take care to make gameid LSB first - GJP
  WORD nodesfound;  // these two are only compared to each other so it
  WORD nodeswanted; // does not matter what internal storage you use
} setupdata_t;

typedef struct {
  UBYTE network[4];             /* high-low */
  UBYTE node[6];                /* high-low */
} localadr_t;

typedef struct {
  UBYTE node[6];                /* high-low */
} nodeadr_t;

// I think a version I downloaded in late 1997, used just one fragment,
// but multiple fragments is supported by AMIPX, I even endorse it - GJP 

// time is used by the communication driver to sequence packets returned
// to DOOM when more than one is waiting
// this is were the 68k has to be very careful to store the time LSB first- GJP

typedef struct {
  struct AMIPX_ECB ecb;      /* too small!!!  need space for 2 fragments !!! */
  struct AMIPX_Fragment dummy;  /* maybe this will fix it */
  struct AMIPX_PacketHeader ipx;
  long time;
  doomdata_t data;
} packet_t;

#pragma options align=power

struct AMIPX_Library *AMIPX_Library = NULL;
static packet_t IPX_packets[IPX_NUMPACKETS];
static nodeadr_t IPX_nodeadr[MAXNETNODES+1];  // first is local, last is broadcast
static nodeadr_t IPX_remoteadr;               // set by each GetPacket
static localadr_t IPX_localadr;        // set at startup
static UWORD IPX_socketid = 0;
static long IPX_localtime;          // for time stamp in packets
static long IPX_remotetime;
static BOOL IPX_got_a_packet;

/**********************************************************************/
static int IPX_OpenSocket (WORD socketNumber)
{
  int outsock;

  if ((outsock = AMIPX_OpenSocket (socketNumber)) == 0)
    I_Error ("AMIPX_OpenSocket() failed");
  return outsock;
}

/**********************************************************************/
static void IPX_ListenForPacket (struct AMIPX_ECB *ecb)
{
  int retval;

  if ((retval = AMIPX_ListenForPacket (ecb)))
    I_Error ("ListenForPacket: 0x%x", retval);
}

/**********************************************************************/

#if 0

static void print_address (char *msg, struct AMIPX_Address *adr)
{
  printf ("   %s Network:         %02x:%02x:%02x:%02x\n", msg,
          adr->Network[0], adr->Network[1],
          adr->Network[2], adr->Network[3]);
  printf ("   %s Node:            %02x:%02x:%02x:%02x:%02x:%02x\n", msg,
          adr->Node[0], adr->Node[1], adr->Node[2],
          adr->Node[3], adr->Node[4], adr->Node[5]);
  printf ("   %s Socket:          %04x\n", msg, adr->Socket);
}

/**********************************************************************/
static void print_ipx (struct AMIPX_PacketHeader *ipx)
{
  printf ("IPX Packet header at $%08x\n", ipx);
  printf ("  Checksum:             %04x\n", ipx->Checksum);
  printf ("  Length:               %04x\n", ipx->Length);
  printf ("  Tc:                   %02x\n", ipx->Tc);
  printf ("  Type:                 %02x\n", ipx->Type);
  print_address ("Dst", &ipx->Dst);
  print_address ("Src", &ipx->Src);
}

/**********************************************************************/
static void print_ecb (struct AMIPX_ECB *ecb)
{
  int i;

  printf ("ECB   at $%08x\n", ecb);
  printf ("  Link:                 %08x\n", ecb->Link);
  printf ("  ESR:                  %08x\n", ecb->ESR);
  printf ("  InUse:                %02x\n", ecb->InUse);
  printf ("  CompletionCode:       %02x\n", ecb->CompletionCode);
  printf ("  Socket:               %04x\n", ecb->Socket);
  printf ("  IPXWork:              %02x:%02x:%02x:%02x\n",
          ecb->IPXWork[0], ecb->IPXWork[1],
          ecb->IPXWork[2], ecb->IPXWork[3]);
  printf ("  ImmedAddr:            %02x:%02x:%02x:%02x:%02x:%02x\n",
          ecb->ImmedAddr[0], ecb->ImmedAddr[1],
          ecb->ImmedAddr[2], ecb->ImmedAddr[3],
          ecb->ImmedAddr[4], ecb->ImmedAddr[5]);
  printf ("  FragCount:            %04x\n", ecb->FragCount);
  for (i = 0; i < ecb->FragCount; i++) {
    printf ("  Fragment[%d].FragData: %08x\n", i, ecb->Fragment[i].FragData);
    printf ("  Fragment[%d].FragSize: %04x\n", i,
            ecb->Fragment[i].FragSize);
  }
}

#endif

/**********************************************************************/
// Send the data defined by doomcom->data and doomcom->datalength
// to the node doomcom->remotenode.
// Broadcast if doomcom->remotenode == MAXNETNODES

static void IPX_PacketSend (void)
{
  int j, c, retval;
  int destination;
  int len;
  doomdata_t sw;

  // byte swap if not in setup
  if (IPX_localtime != -1) {
    sw.checksum = SWAPLONG(netbuffer->checksum);
    sw.player = netbuffer->player;
    sw.retransmitfrom = netbuffer->retransmitfrom;
    sw.starttic = netbuffer->starttic;
    sw.numtics = netbuffer->numtics;

    for (c = 0 ; c < netbuffer->numtics; c++) {
      sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
      sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
      sw.cmds[c].angleturn = SWAPSHORT(netbuffer->cmds[c].angleturn);
      sw.cmds[c].consistancy = SWAPSHORT(netbuffer->cmds[c].consistancy);
      sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
      sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
    }
    IPX_packets[0].ecb.Fragment[1].FragData = (UBYTE *)&sw;
  } else
    IPX_packets[0].ecb.Fragment[1].FragData = (UBYTE *)&doomcom->data;

  destination = doomcom->remotenode;

// set the time
  IPX_packets[0].time = SWAPLONG(IPX_localtime); // Amiga puts MSB first

// set the address
  for (j = 0; j < 6; j++)
    IPX_packets[0].ipx.Dst.Node[j] = IPX_packets[0].ecb.ImmedAddr[j]
                                   = IPX_nodeadr[destination].node[j];

// set the length (ipx + time + datalength)
  len = sizeof(struct AMIPX_PacketHeader) + sizeof(long) + doomcom->datalength
        + 4;
  IPX_packets[0].ipx.Checksum = 0xffff;
  IPX_packets[0].ipx.Length = len;
  IPX_packets[0].ipx.Type = 4;
  IPX_packets[0].ecb.Fragment[0].FragSize = sizeof(struct AMIPX_PacketHeader)
                                            + sizeof(long);
  IPX_packets[0].ecb.Fragment[1].FragSize = doomcom->datalength + 4;

// send the packet
/*
  printf ("Sending ");
  print_ecb (&(IPX_packets[0].ecb));
  printf ("with IPX header ");
  print_ipx ((struct AMIPX_PacketHeader *)
             IPX_packets[0].ecb.Fragment[0].FragData);
*/
  if ((retval = AMIPX_SendPacket (&(IPX_packets[0].ecb))) != 0)
    I_Error ("SendPacket: 0x%x", retval);

  while(IPX_packets[0].ecb.InUse != 0) {
// IPX Relinquish Control - polled drivers MUST have this here!
    AMIPX_RelinquishControl ();
  }
}

/**********************************************************************/
// If there are no packets to receive, set IPX_got_a_packet = FALSE,
//   and doomcom->remotenode = -1.
// Otherwise set IPX_got_a_packet to TRUE and return packet in
//   doomcom->data and doomcom->datalen.
// Set IPX_remoteadr to address of remote node.
// If we know who it came from, set doomcom->remotenode accordingly,
//   otherwise set doomcom->remotenode = -1.

static void IPX_PacketGet (void)
{
  int packetnum;
  int i, c;
  long besttic;
  packet_t *packet;
  doomdata_t *sw;

// if multiple packets are waiting, return them in order by time

  IPX_got_a_packet = FALSE;
  besttic = MAXLONG;
  packetnum = -1;
  doomcom->remotenode = -1;

  /* printf ("Looking for received packets...\n"); */

  for (i = 1; i < IPX_NUMPACKETS; i++)
    if (!IPX_packets[i].ecb.InUse) {

      /* printf ("\nGOT A PACKET!!!\n"); */

      if ((IPX_localtime != -1 && IPX_packets[i].time == -1))
        IPX_ListenForPacket (&IPX_packets[i].ecb); // unwanted packet
      else if (SWAPLONG(IPX_packets[i].time) < besttic) {
        besttic = SWAPLONG(IPX_packets[i].time);
        packetnum = i;
      }
    }

  if (besttic == MAXLONG)
    return;                           // no packets

//
// got a good packet
//
  IPX_got_a_packet = TRUE;
  packet = &IPX_packets[packetnum];
  IPX_remotetime = besttic;

  if (packet->ecb.CompletionCode)
    I_Error ("IPX_PacketGet: ecb.CompletionCode = 0x%x",
             packet->ecb.CompletionCode);
// corrected that ancient typo, sorry - GJP
// set IPX_remoteadr to the sender of the packet
  memcpy (&IPX_remoteadr, packet->ipx.Src.Node, sizeof(IPX_remoteadr));
  for (i = 0; i < doomcom->numnodes; i++)
    if (!memcmp(&IPX_remoteadr, &IPX_nodeadr[i], sizeof(IPX_remoteadr)))
      break;
  if (i < doomcom->numnodes)
    doomcom->remotenode = i;
  else {
    if (IPX_localtime != -1) {    // this really shouldn't happen
      IPX_ListenForPacket (&packet->ecb);
      return;
    }
  }

// copy out the data
  doomcom->datalength = packet->ipx.Length - sizeof(struct AMIPX_PacketHeader)
                        - sizeof(long) - 4;
  // byte swap if not in setup time
  if (IPX_localtime != -1) {
    sw = &packet->data;
    netbuffer->checksum = SWAPLONG(sw->checksum);
    netbuffer->player = sw->player;
    netbuffer->retransmitfrom = sw->retransmitfrom;
    netbuffer->starttic = sw->starttic;
    netbuffer->numtics = sw->numtics;

    for (c = 0; c < netbuffer->numtics; c++) {
      netbuffer->cmds[c].forwardmove = sw->cmds[c].forwardmove;
      netbuffer->cmds[c].sidemove = sw->cmds[c].sidemove;
      netbuffer->cmds[c].angleturn = SWAPSHORT(sw->cmds[c].angleturn);
      netbuffer->cmds[c].consistancy = SWAPSHORT(sw->cmds[c].consistancy);
      netbuffer->cmds[c].chatchar = sw->cmds[c].chatchar;
      netbuffer->cmds[c].buttons = sw->cmds[c].buttons;
    }
  } else
    memcpy (&doomcom->data, &packet->data, doomcom->datalength);

// repost the ECB
  IPX_ListenForPacket (&packet->ecb);
}

/**********************************************************************/
static void IPX_LookForNodes (int numnetnodes)
{
  int i;
  unsigned int clock[2];
  int oldsec;
  int currsec;
  setupdata_t *dest;
  int total, console;
  static setupdata_t nodesetup[MAXNETNODES];

//
// wait until we get [numnetnodes] packets, then start playing
// the playernumbers are assigned by netid
//
  printf ("Attempting to find all players for %i player net play. "
          "Press CTRL/C to exit.\n", numnetnodes);

  printf ("Looking for a node...\n");

  oldsec = -1;
  IPX_localtime = -1;          // in setup time, not game time

//
// build local setup info
//
  nodesetup[0].nodesfound = 1;
  nodesetup[0].nodeswanted = numnetnodes;
  doomcom->numnodes = 1;

  for (;;) {
    //
    // check for aborting
    //
    chkabort ();

    //
    // listen to the network
    //
    for (;;) {

      IPX_PacketGet ();

      if (!IPX_got_a_packet)
        break;

      if (doomcom->remotenode == -1) {     // it's from a new address
        dest = &nodesetup[doomcom->numnodes];
      } else {                        // it's from a node we already know about
        dest = &nodesetup[doomcom->remotenode];
      }

      if (IPX_remotetime != -1) {   // an early game packet, not a setup packet
        if (doomcom->remotenode == -1)
          I_Error ("Got an unknown game packet during setup");
        // if it allready started, it must have found all nodes
        dest->nodesfound = dest->nodeswanted;  // both swapped
        continue;
      }

      // update setup info
      memcpy (dest, &doomcom->data, sizeof(*dest));

      if (doomcom->remotenode == -1) {     // it's from a new address

        memcpy (&IPX_nodeadr[doomcom->numnodes], &IPX_remoteadr,
                sizeof(IPX_nodeadr[doomcom->numnodes]));
        //
        // if this node has a lower address, take all startup info
        //
        if (memcmp(&IPX_remoteadr, &IPX_nodeadr[0], sizeof(&IPX_remoteadr))
                                                                         < 0) {
        }  // No action ?!
           // You could call this a bug - how does one DOOM know
           // whether everyone wants the same number of players?
           // However, because of this, using internal storage for
           // these setup packets actually works.
           // (which saves the Mac version, because if this was not
           // ignored, the PC would start looking for a multiple of
           // 256 players) - GJP

        doomcom->numnodes++;

        printf ("\nFound node [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                IPX_remoteadr.node[0], IPX_remoteadr.node[1],
                IPX_remoteadr.node[2], IPX_remoteadr.node[3],
                IPX_remoteadr.node[4], IPX_remoteadr.node[5]);

        if (doomcom->numnodes < numnetnodes)
          printf ("Looking for a node...\n");

      } /* end if (doomcom->remotenode == -1) */

    } /* end for (;;) until no more packets received */

    //
    // we are done if all nodes have found all other nodes
    //
    for (i = 0; i < doomcom->numnodes; i++)
      if (nodesetup[i].nodesfound != nodesetup[i].nodeswanted) // both swapped
        break;

    // You will notice that nodesetup[0].nodesfound is never compared to
    // nodesetup[i].nodeswanted  
    if (i == nodesetup[0].nodeswanted)
      break;         // got them all

    //
    // send out a broadcast packet every second
    //
    ppctimer (clock);
    currsec = (clock[1] / (bus_clock>>2));
    if (currsec != oldsec) {
      oldsec = currsec;

      printf (".");
      fflush (stdout);

      nodesetup[0].nodesfound = doomcom->numnodes;
      memcpy (&doomcom->data, &nodesetup[0], sizeof(setupdata_t));
      doomcom->remotenode = MAXNETNODES;
      doomcom->datalength = sizeof(setupdata_t);
      IPX_PacketSend ();     // send to all
    }

  } /* end for (;;) until all nodes have found all other nodes */

//
// count players
//
  total = 0;
  console = 0;

  for (i = 0; i < numnetnodes; i++) {
    if (nodesetup[i].drone)
      continue;
    total++;
    if (total > MAXPLAYERS)
      I_Error ("More than %i players specified!", MAXPLAYERS);
    if (M_CheckParm ("-reverseipx")) {
      if (memcmp (&IPX_nodeadr[i], &IPX_nodeadr[0], sizeof(IPX_nodeadr[0])) > 0)
        console++;
    } else {
      if (memcmp (&IPX_nodeadr[i], &IPX_nodeadr[0], sizeof(IPX_nodeadr[0])) < 0)
        console++;
    }
  }

  if (!total)
    I_Error ("No players specified for game!");

  doomcom->consoleplayer = console;
  doomcom->numplayers = total;

  printf ("Console is player %i of %i\n", console+1, total);
}

/**********************************************************************/
void IPX_InitNetwork (int p)
{
  int i, socket;

//
// get IPX function address
//
  if ((AMIPX_Library = (struct AMIPX_Library *)OpenLibrary
                                                 ("amipx.library",0L)) == NULL)
    I_Error ("Can't open amipx.library");

//
// allocate a socket for sending and receiving
//
  socket = 0x869b;
  i = M_CheckParm ("-socket");
  if (i && i < myargc - 1) {
    socket = atoi (myargv[i+1]);
  }
  IPX_socketid = IPX_OpenSocket (socket);
  printf ("Using IPX socket 0x%04x\n", IPX_socketid);

  AMIPX_GetLocalAddr ((BYTE *)&IPX_localadr);
  printf ("Local address is [%02x:%02x:%02x:%02x:%02x:%02x]\n",
          IPX_localadr.node[0], IPX_localadr.node[1], IPX_localadr.node[2],
          IPX_localadr.node[3], IPX_localadr.node[4], IPX_localadr.node[5]);

//
// set up several receiving ECBs
//
  memset (IPX_packets, 0, IPX_NUMPACKETS * sizeof(packet_t));

  for (i = 1; i < IPX_NUMPACKETS; i++) {
    IPX_packets[i].ecb.Socket = IPX_socketid;
    IPX_packets[i].ecb.FragCount = 2;
    IPX_packets[i].ecb.Fragment[0].FragData = (BYTE *)&IPX_packets[i].ipx;
    IPX_packets[i].ecb.Fragment[0].FragSize = sizeof(struct AMIPX_PacketHeader)
                                            + sizeof(long);
    IPX_packets[i].ecb.Fragment[1].FragData = (BYTE *)&IPX_packets[i].data;
    IPX_packets[i].ecb.Fragment[1].FragSize = sizeof(doomdata_t);
    IPX_ListenForPacket (&IPX_packets[i].ecb);
  }

//
// set up a sending ECB
//
  memset (&IPX_packets[0],0,sizeof(IPX_packets[0]));

  IPX_packets[0].ecb.Socket = IPX_socketid;
  IPX_packets[0].ecb.FragCount = 2;
  IPX_packets[0].ecb.Fragment[0].FragData = (BYTE *)&IPX_packets[0].ipx;
  IPX_packets[0].ecb.Fragment[1].FragData = (BYTE *)&doomcom->data;
  memcpy (IPX_packets[0].ipx.Dst.Network, IPX_localadr.network, 4);
  IPX_packets[0].ipx.Dst.Socket = IPX_socketid;

  /* why doesn't amipx.library fill in ipx.Src? */
  memcpy (&IPX_packets[0].ipx.Src, &IPX_localadr, 4 + 6);
  IPX_packets[0].ipx.Src.Socket = IPX_socketid;

// known local node at 0
  memcpy (IPX_nodeadr[0].node, IPX_localadr.node, 6);

// broadcast node at MAXNETNODES
  memset (IPX_nodeadr[MAXNETNODES].node, 0xff, 6);

  netsend = IPX_PacketSend;
  netget = IPX_PacketGet;
  netgame = true;

  // parse player number and host list

  IPX_LookForNodes (myargv[p+1][0] - '0');

  IPX_localtime = 0;

  doomcom->id = DOOMCOM_ID;
}

/**********************************************************************/
static void IPX_Shutdown (void)
{
  if (IPX_socketid != 0) {
    printf ("IPX_Shutdown: Closing socket and library\n");
    AMIPX_CloseSocket (IPX_socketid);
    IPX_socketid = 0;
  }
  if (AMIPX_Library != NULL) {
    CloseLibrary ((struct Library *)AMIPX_Library);
    AMIPX_Library = NULL;
  }
}

/**********************************************************************/

#endif  /* IPX */

/**********************************************************************/
/**********************************************************************/
//
// I_InitNetwork
//
void I_InitNetwork (void)
{
  int i;
  int p;

  doomcom = malloc (sizeof (*doomcom) );
  memset (doomcom, 0, sizeof(*doomcom) );

  // set up for network
  i = M_CheckParm ("-dup");
  if (i && i < myargc - 1) {
    doomcom->ticdup = myargv[i+1][0]-'0';
    if (doomcom->ticdup < 1)
      doomcom->ticdup = 1;
    if (doomcom->ticdup > 9)
      doomcom->ticdup = 9;
  } else
    doomcom-> ticdup = 1;

  if (M_CheckParm ("-extratic"))
    doomcom-> extratics = 1;
  else
    doomcom-> extratics = 0;

  p = M_CheckParm ("-port");
  if (p && p < myargc - 1) {
    IP_DOOMPORT = atoi (myargv[p+1]);
    printf ("using alternate port %i\n",IP_DOOMPORT);
  }

  // parse network game options,
  //  -net <consoleplayer> <host> <host> ...
  if ((i = M_CheckParm ("-net")) != 0) {

    IP_InitNetwork (i);

  } else if ((i = M_CheckParm ("-netserial")) != 0) {

    SER_InitNetwork (i);

#ifdef AMIPX

  } else if ((i = M_CheckParm ("-netipx")) != 0) {

    IPX_InitNetwork (i);

#endif

  } else {

    // single player game
    netgame = false;
    doomcom->id = DOOMCOM_ID;
    doomcom->numplayers = doomcom->numnodes = 1;
    doomcom->deathmatch = false;
    doomcom->consoleplayer = 0;

  }
}


/**********************************************************************/
void I_NetCmd (void)
{
  if (doomcom->command == CMD_SEND) {
#ifdef AMIPX
    IPX_localtime++;
#endif
    netsend ();
  } else if (doomcom->command == CMD_GET) {
    netget ();
  } else
    I_Error ("Bad net cmd: %i\n",doomcom->command);
}

/**********************************************************************/
void _STDcleanup_net (void)
{
  IP_Shutdown ();
  SER_Shutdown ();
#ifdef AMIPX
  IPX_Shutdown ();
#endif
}

/**********************************************************************/
