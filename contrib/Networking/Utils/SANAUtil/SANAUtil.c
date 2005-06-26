/*
 * SANAUtil - A simple SANA-II utility program
 *
 * Copyright (C) 1992-1995 Timo Rossi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Version 0.39 -- 5.3.05 (by Neil Cafferkey)
 *  - Changed to BSD-style licence by kind permission of Timo Rossi.
 *  - Removed SAS/C dependencies.
 *
 * Version 0.38 -- 4.8.95
 *  - now DUMP/RECEIVE work with all device types
 *  - document file created.
 *
 * Version 0.37 -- 18.07.94
 *  - added RECEIVE
 *
 * Version 0.36 -- 14.02.94
 *
 * Version 0.34
 *  - DATA-keyword allows sending of ASCII strings
 *  - reads device & unit from env. vars SANA2.Device & SANA2.Unit
 *  - multicast packet length & type setting fixed
 *  - dump length fixed
 *
 */

#include <exec/types.h>
#include <exec/errors.h>
#include <exec/memory.h>
#include <exec/devices.h>
#include <dos/dos.h>
#include <dos/rdargs.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/alib.h>

#include <stdlib.h>
#include <string.h>

#ifndef REG
#ifdef __mc68000
#define _REG(A, B) B __asm(#A)
#define REG(A, B) _REG(A, B)
#else
#define REG(A, B) B
#endif
#endif

#ifdef __SASC
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
#define exit XCEXIT
void MemCleanup(void) {}
#define ALIGNED __aligned
#endif

#include <devices/sana2.h>

#define VERSION "0"
#define REVISION "39"
#define DATE "5.3.2005"

#ifdef __AROS__
#define DEFAULT_DEVICE_NAME "etherlink3.device"
#else
#define DEFAULT_DEVICE_NAME "a2065.device"
#endif
#define DEFAULT_UNIT_NUMBER 0

#define DEFAULT_PACKET_TYPE 5000
#define DEFAULT_PACKET_LEN 100

#define MAX_SPECIALSTATS 10

struct SpecialStatData {
  struct Sana2SpecialStatHeader hdr;
  struct Sana2SpecialStatRecord stat[MAX_SPECIALSTATS];
};

char ProgName[] = "SANAutil";

static const char VersionString[] =
   "$VER: SANAutil " VERSION "." REVISION " (" DATE ")";

char Prog_Template[] =
  "-D=DEVICE/K,-U=UNIT/N/K,-A=ADDRESS/K,"
  "-E=EXCLUSIVE/S,-P=PROMISCUOUS/S,"
  "SEND/K,TYPE/N/K,LENGTH/N/K,DATA/K,REC=RECEIVE/K/N,"
  "BROADCAST/S,DUMP/S,STATUS/S,CONFIG/S,ONLINE/S,OFFLINE/S,"
  "MULTICAST/K,ADDMULTICAST/K,DELMULTICAST/K,"
  "TRACKTYPE/N/K,UNTRACKTYPE/N/K,TYPESTAT/N/K";

enum Prog_Args {
  DEVICE_ARG,
  UNIT_ARG,
  ADDR_ARG,
  EXCL_ARG,
  PROM_ARG,
  SEND_ARG,
  TYPE_ARG,
  LENGTH_ARG,
  DATA_ARG,
  RECEIVE_ARG,
  BROADCAST_ARG,
  DUMP_ARG,
  STATUS_ARG,
  CONFIG_ARG,
  ONLINE_ARG,
  OFFLINE_ARG,
  MULTICAST_ARG,
  ADDMULTICAST_ARG,
  DELMULTICAST_ARG,
  TRACKTYPE_ARG,
  UNTRACKTYPE_ARG,
  TYPESTAT_ARG,
  NUM_ARGS
};

struct RDArgs *ProgArgs = NULL;

struct MsgPort *SanaPort = NULL;
struct IOSana2Req *SanaReq = NULL;
struct Device *SanaDev = NULL;
UBYTE *PacketBuf = NULL;
long PacketBufSize;

/*********/

static int stch_i(const char *str, int *result);

static int MemCopy(REG(a0, UBYTE *to), REG(a1, UBYTE *from), REG(d0, LONG len))
{
  CopyMem(from, to, len);
  return 1;
}

/*
 * Hex dump
 *
 */
static void HexDump(UBYTE *buf, LONG length)
{
   LONG l2, offset = 0;
   int i;

   while(length)
     {
       l2 = length;
       if(l2 > 16) l2 = 16;
       /* hexdump buffer */
       Printf("%06lx:", offset);
       for(i = 0; i < l2; i++) Printf(" %02lx", buf[i]);
       PutStr("\n");
       /* --- */
       buf += l2;
       length -= l2;
       offset += l2;
     }
}

/*
 * Display an address (length in bytes as argument)
 *
 */
static void ShowAddress(UBYTE *addr, int len)
{
  len--;
  while(len--)
    Printf("%02lx:", *addr++);
  Printf("%02lx", *addr);
}

/*
 * Parse an address and store it to *adr, return TRUE if successful,
 * FALSE if not.
 *
 */
static int ParseAddr(char *str, UBYTE *adr)
{
  int i, k, n;

  memset(adr, 0, SANA2_MAX_ADDR_BYTES); /* clear address */

  for(i=0; i<SANA2_MAX_ADDR_BYTES; i++)
    {
      if((k = stch_i(str, &n)) == 0)
        return FALSE;
      *adr++ = n;
      str += k;
      if(*str == '\0')
        break;
      if(i < SANA2_MAX_ADDR_BYTES-1 && *str++ != ':')
        return FALSE;
    }
  return TRUE;
}

/*
 * fill a data buffer with the contents of a string
 *
 */
static void StringFill(char *addr, int len, char *str)
{
  char *s = str;

  while(len--)
    {
      *addr++ = *s++;
      if(!*s) s = str;
    }
}

/*********/

/*
 * Display error message
 *
 */
static void Sana2ErrorMsg(struct IOSana2Req *req)
{
  PutStr(" -- Error: ");
  switch(req->ios2_Req.io_Error)
    {
      case IOERR_OPENFAIL:
        PutStr("Device open failure");
        break;

      case IOERR_ABORTED:
        PutStr("IO Request aborted");
        break;

      case IOERR_NOCMD:
        PutStr("Command not supported");
        break;

      case IOERR_BADLENGTH:
        PutStr("Not a valid length");
        break;

      case IOERR_BADADDRESS:
        PutStr("Invalid address");
        break;

      case IOERR_UNITBUSY:
        PutStr("Unit is busy");
        break;

      case IOERR_SELFTEST:
        PutStr("Hardware failed self-test");
        break;

      case S2ERR_NO_ERROR:
        PutStr("No error (should not happen)");
        break;

      case S2ERR_NO_RESOURCES:
        PutStr("Resource allocation failure");
        break;

      case S2ERR_BAD_ARGUMENT:
        PutStr("Bad argument");
        break;

      case S2ERR_BAD_STATE:
        PutStr("Bad state");
        break;

      case S2ERR_BAD_ADDRESS:
        PutStr("Bad address");
        break;

      case S2ERR_MTU_EXCEEDED:
        PutStr("Maximum transfer length exceeded");
        break;

      case S2ERR_NOT_SUPPORTED:
        PutStr("Command not supported");
        break;

      case S2ERR_SOFTWARE:
        PutStr("Software error");
        break;

      case S2ERR_OUTOFSERVICE:
        PutStr("Driver is offline");
        break;

      default:
        Printf("%ld", req->ios2_Req.io_Error);
        break;
    }

  if(req->ios2_WireError != S2WERR_GENERIC_ERROR)
    {
      PutStr("\nWireError: ");

      switch(req->ios2_WireError)
        {
          case S2WERR_GENERIC_ERROR:
            PutStr("(no specific information)");
            break;

          case S2WERR_NOT_CONFIGURED:
            PutStr("Unit is not configured");
            break;

          case S2WERR_UNIT_ONLINE:
            PutStr("Unit is currently online");
            break;

          case S2WERR_UNIT_OFFLINE:
            PutStr("Unit is currently offline");
            break;

          case S2WERR_ALREADY_TRACKED:
            PutStr("Protocol is already being tracked");
            break;

          case S2WERR_NOT_TRACKED:
            PutStr("Protocol is not being tracked");
            break;

          case S2WERR_BUFF_ERROR:
            PutStr("Buffer management error");
            break;

          case S2WERR_SRC_ADDRESS:
            PutStr("Source address error");
            break;

          case S2WERR_DST_ADDRESS:
            PutStr("Destination address error");
            break;

          case S2WERR_BAD_BROADCAST:
            PutStr("Broadcast error");
            break;

          case S2WERR_BAD_MULTICAST:
            PutStr("Multicast error");
            break;

          case S2WERR_MULTICAST_FULL:
            PutStr("Multicast address list full");
            break;

          case S2WERR_BAD_EVENT:
            PutStr("Unsupported event class");
            break;

          case S2WERR_BAD_STATDATA:
            PutStr("StatData failed sanity check");
            break;

          case S2WERR_IS_CONFIGURED:
            PutStr("Attempt to configure twice");
            break;

          case S2WERR_NULL_POINTER:
            PutStr("Null pointer");
            break;

          default:
            Printf("%ld", req->ios2_WireError);
            break;
        }
    }
  PutStr("\n");
}


static void CleanExit(char *msg, int code)
{
  if(msg != NULL)
    {
      Printf("%s: ", ProgName);
      PutStr(msg);
    }

  if(code != 0)
    PrintFault(code, ProgName);

  if(ProgArgs != NULL)
    FreeArgs(ProgArgs);

  if(SanaDev != NULL)
    CloseDevice((struct IORequest *)SanaReq);

  if(SanaReq != NULL)
    DeleteIORequest((struct IORequest *)SanaReq);

  if(SanaPort != NULL)
    DeleteMsgPort(SanaPort);

  if(PacketBuf != NULL)
    FreeMem(PacketBuf, PacketBufSize);

  exit(msg == NULL && code == 0 ? RETURN_OK : RETURN_ERROR);
}

static void Open_Sana_Device(char *name, int unit, int flags)
{
  static ULONG SanaTags[] = {
    S2_CopyToBuff, (ULONG)MemCopy,
    S2_CopyFromBuff, (ULONG)MemCopy,
    TAG_DONE, 0
  };

  if((SanaPort = CreateMsgPort()) == NULL)
    CleanExit("Can't create MsgPort\n", 0);

  if((SanaReq = (struct IOSana2Req *)CreateIORequest(SanaPort,
     sizeof(struct IOSana2Req))) == NULL)
    CleanExit("Can't create IORequest\n", 0);

  SanaReq->ios2_BufferManagement = SanaTags;

  if(OpenDevice(name, unit, (struct IORequest *)SanaReq, flags) != 0)
    {
      Printf("Can't open device %s unit %ld", name, unit);
      Sana2ErrorMsg(SanaReq);
      CleanExit("", 0);
    }

  SanaDev = SanaReq->ios2_Req.io_Device;
}

static void Config(UBYTE *addr)
{
#ifdef DEBUG
  PutStr("Config\n");
#endif

  if(addr != NULL)
    memcpy(SanaReq->ios2_SrcAddr, addr, SANA2_MAX_ADDR_BYTES);
  else
    {
      SanaReq->ios2_Req.io_Command = S2_GETSTATIONADDRESS;
      if(DoIO((struct IORequest *)SanaReq) != 0)
        {
          PutStr("GetStationAddress failed");
          Sana2ErrorMsg(SanaReq);
          return;
        }

      memcpy(SanaReq->ios2_SrcAddr, SanaReq->ios2_DstAddr,
             SANA2_MAX_ADDR_BYTES);
    }

  SanaReq->ios2_Req.io_Command = S2_CONFIGINTERFACE;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("ConfigInterface failed");
      Sana2ErrorMsg(SanaReq);
    }
}

static void OffLine(void)
{
#ifdef DEBUG
  PutStr("OffLine\n");
#endif

  SanaReq->ios2_Req.io_Command = S2_OFFLINE;

  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("OffLine command failed");
      Sana2ErrorMsg(SanaReq);
    }
}

static void OnLine(void)
{
#ifdef DEBUG
  PutStr("OnLine\n");
#endif

  SanaReq->ios2_Req.io_Command = S2_ONLINE;

  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("OnLine command failed");
      Sana2ErrorMsg(SanaReq);
    }
}

static void Status(void)
{
  static struct Sana2DeviceQuery /*ALIGNED*/ DevQuery;
  static struct Sana2DeviceStats /*ALIGNED*/ DevStat;
  static struct SpecialStatData /*ALIGNED*/ SpecialStats;
  int addr_bytes;
  int i;

  SanaReq->ios2_Req.io_Command = S2_DEVICEQUERY;
  SanaReq->ios2_StatData = &DevQuery;
  DevQuery.SizeAvailable = sizeof(struct Sana2DeviceQuery);
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("DeviceQuery failed");
      Sana2ErrorMsg(SanaReq);
      return;
    }

/*
  Printf("SizeAvailable: %ld, SizeSupplied: %ld\n",
        DevQuery.SizeAvailable, DevQuery.SizeSupplied);
 */

  Printf("QueryFormat: %ld, DeviceLevel: %ld\n",
         DevQuery.DevQueryFormat, DevQuery.DeviceLevel);

  Printf("Address field size: %ld bits, Maximum transfer unit: %ld bytes\n",
         DevQuery.AddrFieldSize, DevQuery.MTU);
  Printf("Speed: %ld bps, HardwareType: ", DevQuery.BPS);

  switch(DevQuery.HardwareType)
    {
      case S2WireType_Ethernet:
        PutStr("Ethernet");
        break;

      case S2WireType_IEEE802:
        PutStr("IEEE802.3");
        break;

      case S2WireType_Arcnet:
        PutStr("ArcNet");
        break;

      case S2WireType_LocalTalk:
        PutStr("LocalTalk");
        break;

      case S2WireType_AmokNet:
        PutStr("AmokNet");
        break;

      case S2WireType_PPP:
        PutStr("PPP");
        break;

      case S2WireType_SLIP:
        PutStr("SLIP");
        break;

      case S2WireType_CSLIP:
        PutStr("Compressed SLIP");
        break;

      default:
        Printf("%ld", DevQuery.HardwareType);
        break;
    }

  PutStr("\n");

  SanaReq->ios2_Req.io_Command = S2_GETSTATIONADDRESS;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      Printf("GetStationAddress failed");
      Sana2ErrorMsg(SanaReq);
      return;
    }

  if(DevQuery.AddrFieldSize > SANA2_MAX_ADDR_BITS)
    {
      PutStr("Address field size too large!\n");
      return;
    }

  if(DevQuery.AddrFieldSize & 7)
    PutStr("Warning: Address size not divisible by 8\n");

  addr_bytes = (DevQuery.AddrFieldSize+7)/8;

  PutStr("Current addr: ");
  ShowAddress(SanaReq->ios2_SrcAddr, addr_bytes);
  PutStr(", Default addr: ");
  ShowAddress(SanaReq->ios2_DstAddr, addr_bytes);
  PutStr("\n");

  SanaReq->ios2_Req.io_Command = S2_GETGLOBALSTATS;
  SanaReq->ios2_StatData = &DevStat;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      Printf("GetGlobalStats failed");
      Sana2ErrorMsg(SanaReq);
    }
  else /* GetGlobalStats successful */
    {
      /*
       * This requires at least revision 1.11 of sana2.h include file
       * (some structure offsets are wrong in older versions)
       */
      Printf("Packets received: %ld, Packets sent: %ld\n",
             DevStat.PacketsReceived, DevStat.PacketsSent);
      Printf("Bad Data: %ld, Overruns: %ld\n",
             DevStat.BadData, DevStat.Overruns);

      Printf("Unknown packets received: %ld, Reconfigurations: %ld\n",
             DevStat.UnknownTypesReceived, DevStat.Reconfigurations);

      PutStr("Last start: ");
      if(DevStat.LastStart.tv_secs == 0)
        PutStr("--\n");
      else
        {
          static struct DateTime dt;
          static char str_day[LEN_DATSTRING];
          static char str_date[LEN_DATSTRING];
          static char str_time[LEN_DATSTRING];

          dt.dat_Stamp.ds_Days = DevStat.LastStart.tv_secs / 86400;
          dt.dat_Stamp.ds_Minute = (DevStat.LastStart.tv_secs  % 86400)
            / 60;
          dt.dat_Stamp.ds_Tick = (DevStat.LastStart.tv_secs % 60) * 50;
          dt.dat_Format = FORMAT_DOS;
          dt.dat_Flags = 0;
          dt.dat_StrDay = str_day;
          dt.dat_StrDate = str_date;
          dt.dat_StrTime = str_time;

          DateToStr(&dt);

          Printf("%s %s %s\n", str_day, str_date, str_time);
        }
     }

  SpecialStats.hdr.RecordCountMax = MAX_SPECIALSTATS;
  SanaReq->ios2_Req.io_Command = S2_GETSPECIALSTATS;
  SanaReq->ios2_StatData = &SpecialStats;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      Printf("GetSpecialStats failed");
      Sana2ErrorMsg(SanaReq);
    }
  else /* GetSpecialStats successful */
    {
      if(SpecialStats.hdr.RecordCountSupplied == 0)
        PutStr("No special stat data\n");
      else
        {
          PutStr("Special stats:\n");
          for(i = 0; i < SpecialStats.hdr.RecordCountSupplied; i++)
            {
              Printf("(%ld/%ld) %s: %ld\n",
                     SpecialStats.stat[i].Type >> 16,
                     SpecialStats.stat[i].Type & 0xffff,
                     SpecialStats.stat[i].String,
                     SpecialStats.stat[i].Count);
            }
        }
    }
}

/*
 * Packet dump -- this may miss a lot of packets,
 * but it is useful in debugging anyway...
 *
 */
static void Dump(void)
{
  static struct Sana2DeviceQuery /*ALIGNED*/ DevQuery;
  int addr_bytes;

  SanaReq->ios2_Req.io_Command = S2_DEVICEQUERY;
  SanaReq->ios2_StatData = &DevQuery;
  DevQuery.SizeAvailable = sizeof(struct Sana2DeviceQuery);
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("DeviceQuery failed");
      Sana2ErrorMsg(SanaReq);
      return;
    }

/*
 * Note that packet buffer needs to be big enough
 * for the packet data and header (and possible CRC)
 * fields, as we use the SANA2IOF_RAW-flag
 *
 * There is no way to ask the sizes of these fields
 * from the sana2 device, so we just assume that they
 * cannot be bigger than 256 bytes
 */
  PacketBufSize = DevQuery.MTU + 256;

  if(DevQuery.AddrFieldSize & 7)
    PutStr("Warning: Address size not divisible by 8\n");

  addr_bytes = (DevQuery.AddrFieldSize+7)/8;

  if((PacketBuf = AllocMem(PacketBufSize, MEMF_CLEAR)) == NULL)
    CleanExit("Out of memory\n", 0);

  PutStr("Waiting for packets...\n");

  for(;;)
    {
      SanaReq->ios2_Req.io_Command = S2_READORPHAN;
      SanaReq->ios2_Req.io_Flags = SANA2IOF_RAW;
      SanaReq->ios2_DataLength = PacketBufSize; /* not really used */
      SanaReq->ios2_Data = PacketBuf;
      BeginIO((struct IORequest *)SanaReq);
      if(Wait(SIGBREAKF_CTRL_C | (1 << SanaPort->mp_SigBit))
         & SIGBREAKF_CTRL_C)
        {
          AbortIO((struct IORequest *)SanaReq);
          WaitIO((struct IORequest *)SanaReq);
          CleanExit("***Break\n", 0);
        }
      if(WaitIO((struct IORequest *)SanaReq) != 0)
        {
          PutStr("ReadOrphan failed");
          Sana2ErrorMsg(SanaReq);
          return;
        }
      else
        {
          PutStr("Packet from ");
          ShowAddress(SanaReq->ios2_SrcAddr, addr_bytes);
          Printf(", length %ld\n", SanaReq->ios2_DataLength);
          HexDump(PacketBuf, SanaReq->ios2_DataLength);
        }
    }
}

/*
 * Receive specific type packets
 *
 */
static void Receive(int type)
{
  static struct Sana2DeviceQuery /*ALIGNED*/ DevQuery;
  int addr_bytes;

  SanaReq->ios2_Req.io_Command = S2_DEVICEQUERY;
  SanaReq->ios2_StatData = &DevQuery;
  DevQuery.SizeAvailable = sizeof(struct Sana2DeviceQuery);
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("DeviceQuery failed");
      Sana2ErrorMsg(SanaReq);
      return;
    }

/* see note in the Dump-function */
  PacketBufSize = DevQuery.MTU + 256;

  if(DevQuery.AddrFieldSize & 7)
    PutStr("Warning: Address size not divisible by 8\n");

  addr_bytes = (DevQuery.AddrFieldSize+7)/8;

  if((PacketBuf = AllocMem(PacketBufSize, MEMF_CLEAR)) == NULL)
    CleanExit("Out of memory\n", 0);

  Printf("Waiting for packets (type %ld (0x%lx)...\n", type, type);

  for(;;)
    {
      SanaReq->ios2_Req.io_Command = CMD_READ;
      SanaReq->ios2_Req.io_Flags = SANA2IOF_RAW;
      SanaReq->ios2_DataLength = PacketBufSize; /* not really used */
      SanaReq->ios2_PacketType = type;
      SanaReq->ios2_Data = PacketBuf;
      BeginIO((struct IORequest *)SanaReq);
      if(Wait(SIGBREAKF_CTRL_C | (1 << SanaPort->mp_SigBit))
         & SIGBREAKF_CTRL_C)
        {
          AbortIO((struct IORequest *)SanaReq);
          WaitIO((struct IORequest *)SanaReq);
          CleanExit("***Break\n", 0);
        }
      if(WaitIO((struct IORequest *)SanaReq) != 0)
        {
          PutStr("CMD_READ failed");
          Sana2ErrorMsg(SanaReq);
          return;
        }
      else
        {
          PutStr("Packet from ");
              ShowAddress(SanaReq->ios2_SrcAddr, addr_bytes);
          Printf(", length %ld\n", SanaReq->ios2_DataLength);
          HexDump(PacketBuf, SanaReq->ios2_DataLength);
        }
    }
}

/*
 * Send packet (handles broadcasts and multicasts)
 *
 */
static void Send(char *addr_str, int type, int len, char *datastr, int flags)
{
  if(datastr == NULL)
    datastr = "SANAutil test packet -- ";

  if(flags & SANA2IOF_BCAST)
    SanaReq->ios2_Req.io_Command =S2_BROADCAST;
  else
    {
      if(!ParseAddr(addr_str, SanaReq->ios2_DstAddr))
        CleanExit("Bad address argument\n", 0);

      if(flags & SANA2IOF_MCAST)
        SanaReq->ios2_Req.io_Command =S2_MULTICAST;
      else
        SanaReq->ios2_Req.io_Command = CMD_WRITE;
    }

  PacketBufSize = len;
  if((PacketBuf = AllocMem(PacketBufSize, MEMF_CLEAR)) == NULL)
    CleanExit("Out of memory\n", 0);

  StringFill(PacketBuf, len, datastr);

#ifdef DEBUG
  Printf("Send: Length: %ld, packet type $%lx\n", len, type);
#endif

  SanaReq->ios2_Data = PacketBuf;
  SanaReq->ios2_DataLength = len;
  SanaReq->ios2_PacketType = type;
  SanaReq->ios2_Req.io_Flags = 0;
  BeginIO((struct IORequest *)SanaReq);
  if(WaitIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("Packet send failed");
      Sana2ErrorMsg(SanaReq);
    }
}

/*
 * Add a multicast address
 *
 */
static void AddMultiCast(char *addr_str)
{
  if(!ParseAddr(addr_str, SanaReq->ios2_SrcAddr))
        CleanExit("Bad address argument\n", 0);

  SanaReq->ios2_Req.io_Command = S2_ADDMULTICASTADDRESS;
  if(DoIO((struct IORequest *)SanaReq) != 0)
     {
        PutStr("AddMultiCast failed");
        Sana2ErrorMsg(SanaReq);
     }
}

/*
 * Remove a multicast address
 *
 */
static void DelMultiCast(char *addr_str)
{
  if(!ParseAddr(addr_str, SanaReq->ios2_SrcAddr))
        CleanExit("Bad address argument\n", 0);

  SanaReq->ios2_Req.io_Command = S2_DELMULTICASTADDRESS;
  if(DoIO((struct IORequest *)SanaReq) != 0)
     {
        PutStr("DelMultiCast failed");
        Sana2ErrorMsg(SanaReq);
     }
}

/*
 * Add type tracking
 *
 */
static void TrackType(LONG type)
{
  SanaReq->ios2_Req.io_Command = S2_TRACKTYPE;
  SanaReq->ios2_PacketType = type;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("TrackType failed");
      Sana2ErrorMsg(SanaReq);
    }
}

/*
 * Remove type tracking
 *
 */
static void UnTrackType(LONG type)
{
  SanaReq->ios2_Req.io_Command = S2_UNTRACKTYPE;
  SanaReq->ios2_PacketType = type;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("UnTrackType failed");
      Sana2ErrorMsg(SanaReq);
    }
}

/*
 * Statistics for a tracked packet type
 *
 */
static void TypeStats(LONG type)
{
  static struct Sana2PacketTypeStats /*ALIGNED*/ typestat;

  SanaReq->ios2_Req.io_Command = S2_GETTYPESTATS;
  SanaReq->ios2_StatData = &typestat;
  SanaReq->ios2_PacketType = type;
  if(DoIO((struct IORequest *)SanaReq) != 0)
    {
      PutStr("GetTypeStats failed");
      Sana2ErrorMsg(SanaReq);
      return;
    }

  Printf("Type %ld stats:\n", type);
  Printf("Packets sent: %ld, Packets received: %ld\n",
         typestat.PacketsSent, typestat.PacketsReceived);
  Printf("Bytes sent: %ld, Bytes received: %ld\n",
         typestat.BytesSent, typestat.BytesReceived);
  Printf("Packets dropped: %ld\n", typestat.PacketsDropped);
}

/*
 * Display an usage/help message
 *
 */
static void Usage(void)
{
  PutStr(
   "SanaUtil " VERSION "." REVISION " © 1992-1995 by Timo Rossi <trossi@jyu.fi>\n"
   "  -d <device_name> or DEVICE <device_name>\t--  select device\n"
   "  -u <unit_number> or UNIT <unit_number>\t--  select unit\n"
   "\t(Prepends 'networks/' to the name if not a full path,\n"
   "\tuses env. vars SANA2.Device and SANA2.Unit if these options\n"
   "\tare not used. Default is " DEFAULT_DEVICE_NAME ", unit 0)\n"
   "  -e or EXCLUSIVE\t\t-- exclusive access mode\n"
   "  -p or PROMISCUOUS\t\t-- promiscuous mode\n"
   "\t(doesn't check destination addresses)\n"
   "  SEND <destination_address>\t-- send a packet\n"
   "  MULTICAST <multicast_address>\t-- send a multicast packet\n"
   "  BROADCAST\t\t\t-- send a broadcast packet\n"
   "\tTYPE <packet_type>\t-- type of packet (decimal, default 5000)\n"
   "\tLENGTH <length>\t\t-- length of packet to send (default 100)\n"
   "\tDATA <string>\t\t-- ASCII data string to send\n"
   "  DUMP\t\t\t-- display received packets (no multiple buffering)\n"
   "  RECEIVE <type>\t-- receive/display specific type packets\n"
   "  STATUS\t\t-- display device information and statistics\n"
   "  CONFIG\t\t-- configure the device (can be done only once)\n"
   "  ONLINE\t\t-- places the device online\n"
   "  OFFLINE\t\t-- places the device offline\n"
   "  ADDMULTICAST <addr>\t-- add a multicast receive address\n"
   "  DELMULTICAST <addr>\t-- remove a multicast receive address\n"
   "  TRACKTYPE <type>\t-- track a new packet type\n"
   "  UNTRACKTYPE <type>\t-- remove packet type tracking\n"
   "  TYPESTAT <type>\t-- display statistics about a tracked packet type\n"
  );
}

static int stch_i(const char *str, int *result)
{
  BOOL finished = FALSE;
  int n = 0;
  char ch;
  const char *p;

  p = str;
  while(!finished)
  {
    ch = *p++;
    n <<= 4;

    if(ch >= '0' && ch <= '9')
      n |= ch - '0';
    else if(ch >= 'A' && ch <= 'F')
      n |= ch - 'A' + 10;
    else if(ch >= 'a' && ch <= 'f')
      n |= ch - 'a' + 10;
    else
    {
      finished = TRUE;
      n >>= 4;
    }
  }
  *result = n;

  return p - str - 1;
}

/*
 * Main program
 *
 */
void main()
{
  static ULONG ArgArray[NUM_ARGS];
  static char dev_name_string[256];
  static char buff[256];
  static LONG dev_unit_num;
  static int dev_flags, type, len;
  char *dev_name;
  char *send_data = NULL;
  UBYTE addr[SANA2_MAX_ADDR_BYTES];
  int k;

  if(DOSBase->dl_lib.lib_Version < 37)
     {
#define OS_VERSION_MSG "This program requires AmigaOS 2.04 or later (V37)\n"
        BPTR out;
        if(out = Output())
          Write(out, OS_VERSION_MSG, sizeof(OS_VERSION_MSG)-1);
        exit(RETURN_FAIL);
     }

  memset(ArgArray, 0, sizeof(ArgArray));
  if((ProgArgs = ReadArgs(Prog_Template, ArgArray, NULL)) == NULL)
    CleanExit(NULL, IoErr());

  if(ArgArray[UNIT_ARG] != 0)
    dev_unit_num = *((LONG *)ArgArray[UNIT_ARG]);
  else if(GetVar("SANA2.Unit", buff, 255, 0) > 0)
    StrToLong(buff, &dev_unit_num);
  else
    dev_unit_num = DEFAULT_UNIT_NUMBER;

  if(ArgArray[DEVICE_ARG] != 0)
    dev_name = (char *)ArgArray[DEVICE_ARG];
  else if(GetVar("SANA2.Device", buff, 255, 0) > 0)
    dev_name = buff;
  else
    dev_name = DEFAULT_DEVICE_NAME;

  if(!strchr(dev_name, ':') && !strchr(dev_name, '/'))
    strcpy(dev_name_string, "networks/");
  else
    dev_name_string[0] = '\0';

  strcat(dev_name_string, dev_name);

  k =        (ArgArray[ONLINE_ARG] != 0) + (ArgArray[OFFLINE_ARG] != 0) +
        (ArgArray[CONFIG_ARG] != 0) + (ArgArray[STATUS_ARG] != 0) +
        (ArgArray[RECEIVE_ARG] != 0) +
        (ArgArray[DUMP_ARG] != 0) + (ArgArray[SEND_ARG] != 0) +
        (ArgArray[BROADCAST_ARG] != 0) + (ArgArray[MULTICAST_ARG] != 0) +
        (ArgArray[ADDMULTICAST_ARG] != 0) + (ArgArray[DELMULTICAST_ARG] != 0) +
        (ArgArray[TRACKTYPE_ARG] != 0) + (ArgArray[UNTRACKTYPE_ARG] != 0) +
        (ArgArray[TYPESTAT_ARG] != 0);

  if(k == 0)
    {
      Usage();
      CleanExit(NULL, 0);
    }

  if(k != 1 ||
     (ArgArray[CONFIG_ARG] == 0 && ArgArray[ADDR_ARG]) ||
     (ArgArray[SEND_ARG] == 0 && ArgArray[BROADCAST_ARG] == 0 &&
      ArgArray[MULTICAST_ARG] == 0 &&
      (ArgArray[LENGTH_ARG] != 0 || ArgArray[TYPE_ARG] != 0)))
    CleanExit("Bad arguments\n", 0);

  if(ArgArray[ADDR_ARG] && !ParseAddr((char *)ArgArray[ADDR_ARG], addr))
    CleanExit("Bad address argument\n", 0);

  dev_flags = 0;
  if(ArgArray[EXCL_ARG])
    dev_flags |= SANA2OPF_MINE;
  if(ArgArray[PROM_ARG])
    dev_flags |= SANA2OPF_PROM;

  if(ArgArray[SEND_ARG] || ArgArray[BROADCAST_ARG] || ArgArray[MULTICAST_ARG])
    {
      if(ArgArray[TYPE_ARG] == 0)
        type = DEFAULT_PACKET_TYPE;
      else
        type = *((LONG *)ArgArray[TYPE_ARG]);

      if(ArgArray[LENGTH_ARG] == 0)
        len = DEFAULT_PACKET_LEN;
      else
        len = *((LONG *)ArgArray[LENGTH_ARG]);

      if(ArgArray[DATA_ARG] != 0)
        send_data = (char *)ArgArray[DATA_ARG];
    }

#ifdef DEBUG
  Printf("Device: %s, Unit %ld, Flags %ld\n",
         dev_name_string, dev_unit_num, dev_flags);
#endif

  Open_Sana_Device(dev_name_string, dev_unit_num, dev_flags);

#ifdef DEBUG
  Printf("Port = $%lx, Req = $%lx, Dev = $%lx\n", SanaPort, SanaReq, SanaDev);
#endif

  if(ArgArray[TRACKTYPE_ARG])
    TrackType(*((LONG *)ArgArray[TRACKTYPE_ARG]));
  else if(ArgArray[UNTRACKTYPE_ARG])
    UnTrackType(*((LONG *)ArgArray[UNTRACKTYPE_ARG]));
  else if(ArgArray[TYPESTAT_ARG])
    TypeStats(*((LONG *)ArgArray[TYPESTAT_ARG]));
  else if(ArgArray[ADDMULTICAST_ARG])
    AddMultiCast((char *)ArgArray[ADDMULTICAST_ARG]);
  else if(ArgArray[DELMULTICAST_ARG])
    DelMultiCast((char *)ArgArray[DELMULTICAST_ARG]);
  else if(ArgArray[DUMP_ARG])
    Dump();
  else if(ArgArray[RECEIVE_ARG])
    Receive(*((LONG *)ArgArray[RECEIVE_ARG]));
  else if(ArgArray[SEND_ARG])
    Send((char *)ArgArray[SEND_ARG], type, len, send_data, 0);
  else if(ArgArray[BROADCAST_ARG])
    Send(NULL, type, len, send_data, SANA2IOF_BCAST);
  else if(ArgArray[MULTICAST_ARG])
    Send((char *)ArgArray[MULTICAST_ARG], type, len, send_data, SANA2IOF_MCAST);
  else if(ArgArray[STATUS_ARG])
    {
      Printf("Device '%s' (version %ld.%ld), unit %ld\n",
             dev_name_string, SanaDev->dd_Library.lib_Version,
             SanaDev->dd_Library.lib_Revision, dev_unit_num);
      Status();
    }
  else if(ArgArray[CONFIG_ARG])
    Config(ArgArray[ADDR_ARG] ? addr : NULL);
  else if(ArgArray[ONLINE_ARG])
    OnLine();
  else if(ArgArray[OFFLINE_ARG])
    OffLine();

  CleanExit(NULL, 0);
}

