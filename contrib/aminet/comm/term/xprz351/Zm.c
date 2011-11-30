/**********************************************************************
 *   Z M . C
 *    ZMODEM protocol primitives
 *    01-19-87  Chuck Forsberg Omen Technology Inc
 *
 * 29 July 89:
 * Major overhaul by Rick Huebner for adaptation to Amiga XPR protocol spec
 *
 * 28 October 89:
 * Converted to Lattice C 5.04
 *
 * 15 November 1991
 * Code added to support CRC-32 by William M. Perkins.
 *
 * Version 2.63, 30 July 1993 build in locale, by Rainer Hess
 **********************************************************************/

#include "xprzmodem_all.h"
#include "Zcrc.h"

#define CATCOMP_NUMBERS
#include "xprzmodem_catalog.h"

static char *frametypes[] =
{
  "Carrier Lost",                /* -3 */
  "TIMEOUT",                        /* -2 */
  "ERROR",                        /* -1 */
#define FTOFFSET 3
  "ZRQINIT",
  "ZRINIT",
  "ZSINIT",
  "ZACK",
  "ZFILE",
  "ZSKIP",
  "ZNAK",
  "ZABORT",
  "ZFIN",
  "ZRPOS",
  "ZDATA",
  "ZEOF",
  "ZFERR",
  "ZCRC",
  "ZCHALLENGE",
  "ZCOMPL",
  "ZCAN",
  "ZFREECNT",
  "ZCOMMAND",
  "ZSTDERR",
  "xxxxx"
#define FRTYPES 22                /* Total number of frame types in this array */
            /*  not including psuedo negative entries */
};

static UBYTE DLE_actions[] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static UBYTE ZDLE_actions[] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**********************************************************
 *      void zsbhdr(struct Vars *v, USHORT type)
 *
 * Send ZMODEM binary header hdr of type type
 **********************************************************/
void
zsbhdr (struct Vars *v, USHORT type)
{
  UBYTE *hdr = v->Txhdr;
  short n;
  USHORT crc;
  ULONG crc32;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  xsendline (v, ZPAD);
  xsendline (v, ZDLE);

  if ((v->Crc32t = v->Txfcs32))        /* zsbh32() */
    {
      xsendline (v, ZBIN32);
      zsendline (v, (UBYTE) type);

#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "zsbh32: %s %lx\n", frametypes[type + FTOFFSET],
                  v->Txpos);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      crc32 = 0xFFFFFFFFL;
      crc32 = UPDC32 (type, crc32);

      for (n = 4; --n >= 0; ++hdr)
      {
        crc32 = UPDC32 ((0377 & *hdr), crc32);
        zsendline (v, *hdr);
      }
      crc32 = ~crc32;
      for (n = 4; --n >= 0;)
        {
          zsendline (v, (int) crc32);
          crc32 >>= 8;
        }
    }
  else
    {
      xsendline (v, ZBIN);
      zsendline (v, (UBYTE) type);

#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "zsbhdr: %s %lx\n", frametypes[type + FTOFFSET],
                  v->Txpos);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      crc = updcrc (type, 0);
      for (n = 4; --n >= 0;)
        {
          zsendline (v, *hdr);
          crc = updcrc (((USHORT) (*hdr++)), crc);
        }

      crc = updcrc (((USHORT) 0), crc);
      crc = updcrc (((USHORT) 0), crc);
      zsendline (v, (UBYTE) (crc >> 8));
      zsendline (v, (UBYTE) crc);
    }
}                                /* End of void zsbhdr() */

/**********************************************************
 *      void zshhdr(struct Vars *v, USHORT type)
 *
 * Send ZMODEM HEX header hdr of type type
 **********************************************************/
void 
zshhdr (struct Vars *v, USHORT type)
{
  UBYTE *hdr = v->Txhdr;
  short n;
  USHORT crc;

#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "zshhdr: %s %lx\n", frametypes[type + FTOFFSET],
              v->Rxbytes);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif
  sendline (v, ZPAD);
  sendline (v, ZPAD);
  sendline (v, ZDLE);
  sendline (v, ZHEX);
  zputhex (v, (UBYTE) type);
  v->Crc32t = 0;

  crc = updcrc (type, 0);
  for (n = 4; --n >= 0;)
    {
      zputhex (v, *hdr);
      crc = updcrc (((USHORT) (*hdr++)), crc);
    }

  crc = updcrc (((USHORT) 0), crc);
  crc = updcrc (((USHORT) 0), crc);
  zputhex (v, (UBYTE) (crc >> 8));
  zputhex (v, (UBYTE) crc);

  /* Make it printable on remote machine */
  sendline (v, '\r');
  sendline (v, '\n');
  /* Uncork the remote in case a fake XOFF has stopped data flow */
  if (type != ZFIN)
    sendline (v, XON);
}                                /* End of void zshhdr() */

/**********************************************************
 *      void zsdata() and void zsda32()
 *
 * Send binary array buf of length length, with ending
 * ZDLE sequence frameend
 **********************************************************/
void 
zsdata (struct Vars *v, short length, USHORT frameend)
{
  UBYTE *buf, *outptr, c;
  USHORT crc;
  ULONG crc32;

#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, v->Crc32t ? "zsda32: length=%ld end=%lx\n"
          : "zsdata: length=%ld end=%lx\n", (long) length, (long) frameend);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif

  buf = v->Pktbuf;
  outptr = v->Outbuf + v->Outbuflen;
  crc32 = 0xFFFFFFFFL;                /* zsda32() */
  crc = 0;

  while (--length >= 0)
    {
      if (v->dzap)  /* If DirectZap enabled */
      { 
/*        c = *buf;
        if (c == ZDLE || c == ZDLEE)
        {
          *outptr++ = ZDLE;
          c ^= 0x40;
        }
        *outptr++ = v->Lastzsent = c;
*/
        switch (ZDLE_actions[c = *buf])
        {
          case 1:
            *outptr++ = ZDLE;
            c ^= 0x40;
          case 0:
            *outptr++ = v->Lastzsent = c;
        }
      }
      else
      {    
        switch (DLE_actions[c = *buf])
        {
          case 2:
            if (v->Lastzsent != '@')
              goto sendit;
            /* Fallthrough */
          case 1:
            *outptr++ = ZDLE;
            c ^= 0x40;
          sendit:
          case 0:
            *outptr++ = v->Lastzsent = c;
        }
      } 
      if (v->Crc32t)                /* zsda32() */
        crc32 = UPDC32 ((0xFF & *buf++), crc32);
      else
        crc = updcrc (((USHORT) (*buf++)), crc);
    }
  *outptr++ = ZDLE;
  *outptr++ = frameend;
  v->Outbuflen = outptr - v->Outbuf;

  if (v->Crc32t)                /* zsda32() */
    {
      crc32 = UPDC32 (frameend, crc32);
      crc32 = ~crc32;
      for (length = 4; --length >= 0;)
        {
          zsendline (v, (int) crc32);
          crc32 >>= 8;
        }
    }
  else
    {
      crc = updcrc (frameend, crc);
      crc = updcrc (((USHORT) 0), crc);
      crc = updcrc (((USHORT) 0), crc);
      zsendline (v, (UBYTE) (crc >> 8));
      zsendline (v, (UBYTE) crc);
    }

  if (frameend == ZCRCW)
    xsendline (v, XON);
}                                /* End of void zsdata() */

/**********************************************************
 *      short zrdata(struct Vars *v, UBYTE *buf, short length)
 *
 * Receive array buf of max length with ending ZDLE sequence
 * and CRC-16.  Returns the ending character or error code.
 **********************************************************/
short
zrdata (struct Vars *v, UBYTE * buf, short length)
{
  short c, d;
  USHORT crc;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->Rxframeind == ZBIN32)
    return zrdat32 (v, buf, length);

  crc = v->Rxcount = 0;
  for (;;)
    {
      if ((c = zdlread (v)) & ~0xFF)
        {
        crcfoo:
          switch (c)
            {
            case GOTCRCE:
            case GOTCRCG:
            case GOTCRCQ:
            case GOTCRCW:
              crc = updcrc (((d = c) & 0xFF), crc);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc = updcrc (c, crc);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc = updcrc (c, crc);
              if (crc & 0xFFFF)
                {
                  strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_DATA_PACKET_CRC_16 ));
                  return ERROR;
                }
#ifdef DEBUGLOG
              xprsprintf (v->Msgbuf, "zrdata: cnt = %ld ret = %lx\n",
                          (long) v->Rxcount, (long) d);
              dlog (v, v->Msgbuf);
              D (DEBUGINFO);
#endif
              return d;
            case GOTCAN:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_SENDER_CANCELED ));
              return ZCAN;
            case TIMEOUT:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_DATA_PACKET_TIMEOUT ));
              return c;
            case RCDO:
              return c;
            default:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_UNRECOG_DATA_PACKET ));
              return c;
            }
        }
      if (--length < 0)
        {
          strcpy (v->Msgbuf, GetLocalString( &li, MSG_DATA_PACKET_TOO_LONG ));
          return ERROR;
        }
      ++v->Rxcount;
      *buf++ = c;
      crc = updcrc (c, crc);
      continue;
    }
}                                /* End of short zrdata() */

/**********************************************************
 *      short zrdat32(struct Vars *v, UBYTE *buf, short length)
 *
 * Receive array buf of max length with ending ZDLE sequence
 * and CRC-32.  Returns the ending character or error code.
 **********************************************************/
short
zrdat32 (struct Vars *v, UBYTE * buf, short length)
{
  short c, d;
  ULONG crc32;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  crc32 = 0xFFFFFFFFL;
  v->Rxcount = 0;

  for (;;)
    {
      if ((c = zdlread (v)) & ~0xFF)
        {
        crcfoo:
          switch (c)
            {
            case GOTCRCE:
            case GOTCRCG:
            case GOTCRCQ:
            case GOTCRCW:
              d = c;
              c &= 0xFF;
              crc32 = UPDC32 (c, crc32);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc32 = UPDC32 (c, crc32);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc32 = UPDC32 (c, crc32);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc32 = UPDC32 (c, crc32);
              if ((c = zdlread (v)) & ~0xFF)
                goto crcfoo;
              crc32 = UPDC32 (c, crc32);
              if (crc32 != 0xDEBB20E3)
                {
                  strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_DATA_PACKET_CRC_32 ));
                  return ERROR;
                }
#ifdef DEBUGLOG
              xprsprintf (v->Msgbuf, "zrdat32: cnt = %ld ret = %lx\n",
                          (long) v->Rxcount, (long) d);
              dlog (v, v->Msgbuf);
              D (DEBUGINFO);
#endif
              return d;
            case GOTCAN:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_SENDER_CANCELED ));
              return ZCAN;
            case TIMEOUT:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_DATA_PACKET_TIMEOUT ));
              return c;
            case RCDO:
              return c;
            default:
              strcpy (v->Msgbuf, GetLocalString( &li, MSG_UNRECOG_DATA_PACKET ));
              return c;
            }
        }
      if (--length < 0)
        {
          strcpy (v->Msgbuf, GetLocalString( &li, MSG_DATA_PACKET_TOO_LONG ));
          return ERROR;
        }
      ++v->Rxcount;
      *buf++ = c;
      crc32 = UPDC32 (c, crc32);
      continue;
    }
}                                /* End of short zrdat32() */

/**********************************************************
 *      short zgethdr(struct Vars *v)
 *
 * Read a ZMODEM header to hdr, either binary or hex.
 *  On success return type of header.
 *  Otherwise return negative on error.
 **********************************************************/
short
zgethdr (struct Vars *v)
{
  short c, cancount;
  long n;
#ifdef DEBUGLOG
  UBYTE msgbuf[128];

  D (DEBUGINFO);
#endif
/*fix to 2xmaxblk */
  n = v->Baud;                        /* Max characters before start of frame */
  cancount = 5;
again:
  v->Rxframeind = v->Rxtype = 0;
  switch (c = noxrd7 (v))
    {
    case RCDO:
    case TIMEOUT:
      goto fifi;
    case CAN:
      if (--cancount <= 0)
        {
          c = ZCAN;
          goto fifi;
        }
    default:
    agn2:
      if (--n <= 0)
        {
          strcpy (v->Msgbuf, GetLocalString( &li, MSG_HEADER_SEARCH_GARBAGE ));
          return ERROR;
        }
      if (c != CAN)
        cancount = 5;
      goto again;
    case ZPAD:                        /* This is what we want. */
      break;
    }
  cancount = 5;
splat:
  switch (c = noxrd7 (v))
    {
    case ZPAD:
      goto splat;
    case RCDO:
    case TIMEOUT:
      goto fifi;
    default:
      goto agn2;
    case ZDLE:                        /* This is what we want. */
      break;
    }

  switch (c = noxrd7 (v))
    {
    case RCDO:
    case TIMEOUT:
      goto fifi;
    case ZBIN:
      v->Rxframeind = ZBIN;
      v->Crc32 = FALSE;
      c = zrbhdr (v);
      break;
    case ZBIN32:
      v->Crc32 = v->Rxframeind = ZBIN32;
      c = zrbhdr32 (v);
      break;
    case ZHEX:
      v->Rxframeind = ZHEX;
      v->Crc32 = FALSE;
      c = zrhhdr (v);
      break;
    case CAN:
      if (--cancount <= 0)
        {
          c = ZCAN;
          goto fifi;
        }
      goto agn2;
    default:
      goto agn2;
    }
  v->Rxpos = rclhdr (v);
fifi:
  switch (c)
    {
    case GOTCAN:
      c = ZCAN;
    case ZNAK:
    case ZCAN:
    case ERROR:
    case TIMEOUT:
    case RCDO:
      xprsprintf (v->Msgbuf, "%s %s ", frametypes[c + FTOFFSET],
                  (c >= 0) ? GetLocalString( &li, MSG_HEADER) : GetLocalString( &li, MSG_ERROR ));
#ifdef DEBUGLOG
    default:
      if (c >= -3 && c <= FRTYPES)
        xprsprintf (msgbuf, "zgethdr: %s @ %ld\n", frametypes[c + FTOFFSET],
                    v->Rxpos);
      else
        xprsprintf (msgbuf, "zgethdr: Unknown type %ld @ %ld\n", (long) c,
                    v->Rxpos);
      dlog (v, msgbuf);
#endif
    }
  return c;
}                                /* End of short zgethdr() */

/**********************************************************
 *      short zrbhdr(struct Vars *v)
 *
 * Receive a binary style header (type and position)
 **********************************************************/
short 
zrbhdr (struct Vars *v)
{
  UBYTE *hdr = v->Rxhdr;
  short c, n;
  USHORT crc;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if ((c = zdlread (v)) & ~0xFF)
    return c;
  v->Rxtype = c;
  crc = updcrc (c, 0);

  for (n = 4; --n >= 0;)
    {
      if ((c = zdlread (v)) & ~0xFF)
        return c;
      crc = updcrc (c, crc);
      *hdr++ = c;
    }
  if ((c = zdlread (v)) & ~0xFF)
    return c;
  crc = updcrc (c, crc);
  if ((c = zdlread (v)) & ~0xFF)
    return c;
  crc = updcrc (c, crc);
  if (crc & 0xFFFF)
    {
      strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_HEADER_CRC_16 ));
      return ERROR;
    }
  return v->Rxtype;
}                                /* End of short zrbhdr() */

/**********************************************************
 *      short zrbhdr32(struct Vars *v)
 *
 * Receive a binary style header (type and position) with
 * 32 bit FCS
 **********************************************************/
short
zrbhdr32 (struct Vars *v)
{
  UBYTE *hdr = v->Rxhdr;
  short c, n;
  ULONG crc32;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if ((c = zdlread (v)) & ~0xFF)
    return c;
  v->Rxtype = c;
  crc32 = 0xFFFFFFFFL;
  crc32 = UPDC32 (c, crc32);
#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "zrbhdr32: c=%x  crc32=%1x\n", c, crc32);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif

  for (n = 4; --n >= 0;)
    {
      if ((c = zdlread (v)) & ~0xFF)
        return c;
      crc32 = UPDC32 (c, crc32);
      *hdr++ = c;
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "zrbhdr32: c=%x  crc32=%1x\n", c, crc32);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
    }
  for (n = 4; --n >= 0;)
    {
      if ((c = zdlread (v)) & ~0xFF)
        return c;
      crc32 = UPDC32 (c, crc32);
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "zrbhdr32: c=%x  crc32=%1x\n", c, crc32);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
    }
  if (crc32 != 0xDEBB20E3)
    {
      strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_HEADER_CRC_32 ));
      return ERROR;
    }
  return v->Rxtype;
}                                /* End of short zrbhdr32() */

/**********************************************************
 *      short zrhhdr(struct Vars *v)
 *
 * Receive a hex style header (type and position)
 **********************************************************/
short
zrhhdr (struct Vars *v)
{
  UBYTE *hdr = v->Rxhdr;
  short c, n;
  USHORT crc;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if ((c = zgethex (v)) < 0)
    return c;
  v->Rxtype = c;
  crc = updcrc (c, 0);

  for (n = 4; --n >= 0;)
    {
      if ((c = zgethex (v)) < 0)
        return c;
      crc = updcrc (c, crc);
      *hdr++ = c;
    }
  if ((c = zgethex (v)) < 0)
    return c;
  crc = updcrc (c, crc);
  if ((c = zgethex (v)) < 0)
    return c;
  crc = updcrc (c, crc);
  if (crc & 0xFFFF)
    {
      strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_HEADER_CRC ));
      return ERROR;
    }
  if (readock (v, 1) == '\r')
    readock (v, 1);                /* Throw away possible cr/lf */
  return v->Rxtype;
}                                /* End of short zrhhdr() */

/**********************************************************
 *      void zputhex(struct Vars *v, UBYTE c)
 *
 * Send a byte as two hex digits
 **********************************************************/
void 
zputhex (struct Vars *v, UBYTE c)
{
  static char digits[] = "0123456789abcdef";

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  sendline (v, digits[(c >> 4) & 0x0F]);
  sendline (v, digits[c & 0x0F]);
}                                /* End of void zputhex() */

/**********************************************************
 *      void zsendline(struct Vars *v, UBYTE c)
 *
 * Send character c with ZMODEM escape sequence encoding.
 * Escape ZDLE, real DLE, XON, XOFF, and CR following @ (Telenet net escape)
 **********************************************************/
void 
zsendline (struct Vars *v, UBYTE c)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->dzap)  /* DirectZap enabled */
  { 
/*
    if (c == ZDLE || c == ZDLEE)
    {
      xsendline (v, ZDLE);
      c ^= 0x40;
    }
    xsendline (v, v->Lastzsent = c);
*/
    switch (ZDLE_actions[c])
    {
      case 1:
        xsendline (v, ZDLE);
        c ^= 0x40;
      case 0:
        xsendline (v, v->Lastzsent = c);
    }
  }
  else
  {    
    switch (DLE_actions[c])
    {
      case 2:
        if (v->Lastzsent != '@')
          goto sendit;
        /* Fallthrough */
      case 1:
        xsendline (v, ZDLE);
        c ^= 0x40;
   sendit:
      case 0:
        xsendline (v, v->Lastzsent = c);
    }
  } 
}                                /* End of void zsendline() */

/**********************************************************
 *      short zgethex(struct Vars *v)
 *
 * Decode two lower case hex digits into an 8 bit byte value
 **********************************************************/
short
zgethex (struct Vars *v)
{
  short c, n;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if ((n = noxrd7 (v)) < 0)
    return n;
  n -= '0';
  if (n > 9)
    n -= ('a' - ':');
  if (n & ~0xF)
    return ERROR;

  if ((c = noxrd7 (v)) < 0)
    return c;
  c -= '0';
  if (c > 9)
    c -= ('a' - ':');
  if (c & ~0xF)
    return ERROR;

  return (short) (n << 4 | c);
}                                /* End of short zgethex() */

/**********************************************************
 *      short zdlread(struct Vars *v)
 *
 * Read a byte, checking for ZMODEM escape encoding
 * including CAN*5 which represents a quick abort.
 **********************************************************/
short 
zdlread (struct Vars *v)
{
  short c;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if ((c = readock (v, v->Rxtimeout)) != ZDLE)
    return c;
  if ((c = readock (v, v->Rxtimeout)) < 0)
    return c;
  if (c == CAN && (c = readock (v, v->Rxtimeout)) < 0)
    return c;
  if (c == CAN && (c = readock (v, v->Rxtimeout)) < 0)
    return c;
  if (c == CAN && (c = readock (v, v->Rxtimeout)) < 0)
    return c;
  switch (c)
    {
    case CAN:
      return GOTCAN;
    case ZCRCE:
    case ZCRCG:
    case ZCRCQ:
    case ZCRCW:
      return (short) (c | GOTOR);
    case ZRUB0:
      return 0x7F;
    case ZRUB1:
      return 0xFF;
    default:
/*      if (v->dzap)  */
/*      { */
/*        return (short) ((c == ZDLEE) ? ZDLE : c); */
/*      } */
/*      else  */
/*      { */

        if ((c & 0x60) == 0x40)
          return (short) (c ^ 0x40);
/*      } */
      break;
    }
  strcpy (v->Msgbuf, GetLocalString( &li, MSG_BAD_ZMODEM_ESC_SEQ ));
  return ERROR;
}                                /* End of short zdlread() */

/**********************************************************
 *      short noxrd7(struct Vars *v)
 *
 * Read a character from the modem line with timeout.
 * Eat parity, XON and XOFF characters.
 **********************************************************/
short 
noxrd7 (struct Vars *v)
{
  short c;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  for (;;)
  {
    if ((c = readock (v, v->Rxtimeout)) < 0)
      return c;
    switch (c &= 0x7F)
  {
        case XON:
        case XOFF:
          continue;
        default:
          return c;
        }
    }
}                                /* short noxrd7() */

/**********************************************************
 *      void stohdr(struct Vars *v, long pos)
 *
 * Store long integer pos in Txhdr
 **********************************************************/
void 
stohdr (struct Vars *v, long pos)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  v->Txhdr[ZP0] = pos;
  pos >>= 8;
  v->Txhdr[ZP1] = pos;
  pos >>= 8;
  v->Txhdr[ZP2] = pos;
  pos >>= 8;
  v->Txhdr[ZP3] = pos;
}                                /* End of void stohdr() */

/**********************************************************
 *      long rclhdr(struct Vars *v)
 *
 * Recover a long integer from a header
 **********************************************************/
long 
rclhdr (struct Vars *v)
{
  long l;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  l = v->Rxhdr[ZP3];
  l = (l << 8) | v->Rxhdr[ZP2];
  l = (l << 8) | v->Rxhdr[ZP1];
  l = (l << 8) | v->Rxhdr[ZP0];
  return l;
}                                /* End of long rclhdr() */
/* End of Zm.c source */
