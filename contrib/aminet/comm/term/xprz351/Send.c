/**********************************************************************
 * Send.c: File transmission routines for xprzmodem.library;
 * Original Version 2.10, 12 February 1991, by Rick Huebner.
 * Based closely on Chuck Forsberg's sz.c example ZModem code,
 * but too pervasively modified to even think of detailing the changes.
 * Released to the Public Domain; do as you like with this code.
 *
 * Version 2.50, 15 November 1991, CRC-32 additions by William M. Perkins.
 * Version 2.51 29, January 1992, RX_timout fix by John Tillema
 * Version 2.52   6 March 1992, Very minor fix with compiled 020 library
 *               by William M. Perkins.
 * Version 2.63, 30 July 1993 build in locale, by Rainer Hess
 * Version 3.1,  17 August 1993, added Auto-Blocksize by Rainer Hess
 * Version 3.2,  10 Nov 1994, restored FTN Support by Robert Williamson
 *
 **********************************************************************/

#include "xprzmodem_all.h"

#define CATCOMP_NUMBERS
#include "xprzmodem_catalog.h"

#ifdef DEBUGLOG
extern void *DebugLog;
#endif

/**********************************************************
 *      long XProtocolSend(struct XPR_IO *xio)
 *
 * Main file transmission routine; called by comm program
 **********************************************************/
long __saveds __asm 
XProtocolSend (register __a0 struct XPR_IO *xio)
{
  struct Vars *v;
  short err;

  /* Perform common setup and initializations */
  if (!(v = setup (xio)))
    return XPRS_FAILURE;

  v->Rxtimeout = v->FTNmode ? 600 : 300;
  v->Wantfcs32 = TRUE;
  v->Rxflags = 0;

  /* first flush the inbound buffer */
  if (v->FTNmode && v->io.xpr_sflush)
  {
#ifdef KDEBUG 
KPrintF("Flushing\n");  
#endif  
        (*v->io.xpr_sflush) ();
  }

#ifdef KDEBUG
KPrintF("\nXProtocolSend\n");
KPrintF("XprBaud:%ld  UserBaud:%ld\n",v->Baud,v->NewBaud);
KPrintF("FtnMode:%s   DirectZap:%s   Nofiles:%s   SmallBlocks:%s\n",
  (v->FTNmode ? "TRUE" : "FALSE"),
  (v->dzap ? "TRUE" : "FALSE"),
  (v->NoFiles ? "TRUE" : "FALSE"),
  (v->SmallBlocks ? "TRUE" : "FALSE"));
KPrintF("Rxtimeout:%ld\n",v->Rxtimeout);
KPrintF("Filebufmax:%ld   Tframlen:%ld  Ksize:%ld\n", v->Filebufmax,v->Tframlen,v->Ksize);
KPrintF("Pktbuf:%ld   Modembuf:%ld  Outbuf:%ld\n",sizeof(v->Pktbuf),sizeof(v->Modembuf),sizeof(v->Outbuf));
#endif

  /* Transfer the files */
  zmputs (v, "rz\r");
  stohdr (v, 0L);
  zshhdr (v, ZRQINIT);
  sendbuf (v);
  if (getzrxinit (v) == ERROR)
    upderr (v, GetLocalString( &li, MSG_UPLOAD_USER_ERROR ), XPRU_UPLOAD);
  else
    sendbatch (v);

  /* Clean up and return */
  if ((err = v->Errcnt))
    upderr (v, GetLocalString( &li, MSG_SKIPPED_DUE_TO_ERRORS ), XPRU_UPLOAD);
  else
    updmsg (v, GetLocalString( &li, MSG_SEND_DONE ), XPRU_UPLOAD);

  if (v->io.xpr_setserial && v->Oldstatus != -1)
    (*v->io.xpr_setserial) (v->Oldstatus);
  FreeMem (v->Filebuf, v->Filebufmax);
  FreeMem (v, (long) sizeof (struct Vars));

#ifdef DEBUGLOG
  if (DebugLog)
    {
      (*v->io.xpr_fclose) ((long) DebugLog);
      DebugLog = NULL;
    }
#endif
#ifdef KDEBUG
KPrintF("XProtcolSend returning:%s\n",err ? "XPRS_FAILURE" : "XPRS_SUCCESS");
#endif
  return (err) ? XPRS_FAILURE : XPRS_SUCCESS;
} /* End of long XProtocolSend() */

/**********************************************************
 *      short getzrxinit(struct Vars *v)
 *
 * Negotiate with receiver to start a file transfer
 **********************************************************/
short
getzrxinit (struct Vars *v)
{
  short n;

  for (n = v->ErrorLimit; --n >= 0;)
    {
      /* Check for abort from comm program */
      if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
        return ERROR;
      switch (zgethdr (v))
      {
        case ZCHALLENGE:        /* Echo receiver's challenge number */
          stohdr (v, v->Rxpos);
          zshhdr (v, ZACK);
          sendbuf (v);
          continue;
        case ZCOMMAND:                /* They didn't see our ZRQINIT; try again */
          stohdr (v, 0L);
          zshhdr (v, ZRQINIT);
          sendbuf (v);
          continue;
        case ZRINIT:                /* Receiver ready; get transfer parameters */
          v->Rxflags = 0xFF & v->Rxhdr[ZF0];
          v->Txfcs32 = (v->Wantfcs32 && (v->Rxflags & CANFC32));
          v->Rxbuflen = ((USHORT) v->Rxhdr[ZP1] << 8) | v->Rxhdr[ZP0];
#ifdef DEBUGLOG
          xprsprintf (v->Msgbuf, "Txfcs32=%ld Rxbuflen=%ld Tframlen=%ld\n",
                 (long) v->Txfcs32, (long) v->Rxbuflen, (long) v->Tframlen);
          dlog (v, v->Msgbuf);
          D (DEBUGINFO);
#endif
#ifdef KDEBUG
KPrintF("ZRINIT: Txfcs32=%ld Rxbuflen=%ld Tframlen=%ld Ksize=%ld\n",
         (long) v->Txfcs32, (long) v->Rxbuflen, (long) v->Tframlen, (long) v->Ksize );
#endif
          /* Use shortest of the two side's max frame lengths */
          if (v->Tframlen && (!v->Rxbuflen || v->Tframlen < v->Rxbuflen))
            v->Rxbuflen = v->Tframlen;
#ifdef DEBUGLOG
          xprsprintf (v->Msgbuf, "Rxbuflen=%ld\n", (long) v->Rxbuflen);
          dlog (v, v->Msgbuf);
          D (DEBUGINFO);
#endif
#ifdef KDEBUG
KPrintF("Adjusted Rxbuflen=%ld Tframlen=%ld\n", (long) v->Rxbuflen, (long) v->Tframlen);
#endif
          return OK;
        case ZCAN:
        case RCDO:
        case TIMEOUT:
          upderr (v, v->Msgbuf, XPRU_UPLOAD);
          return ERROR;
        case ZRQINIT:
          if (v->Rxhdr[ZF0] == ZCOMMAND)
            continue;
          /* fallthrough... */
        default:
          zshhdr (v, ZNAK);
          sendbuf (v);
          continue;
        }
    }
  return ERROR;
}                                /* End of short getzrxinit() */

/**********************************************************
 *      void sendbatch(struct Vars *v)
 *
 * Send a batch of files
 **********************************************************/
void
sendbatch (struct Vars *v)
{
  UBYTE single, done = FALSE;
  long fstate;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

   /* Lets just clear this in case the host program fails to set this */
   v->Filcnt = 0; /* Up to now we have not sent any files */

  /* If template routines not provided, must be single filename */
  if (!v->io.xpr_ffirst || !v->io.xpr_fnext)
  {
    single = TRUE;
    strcpy (v->Filename, v->io.xpr_filename);
    if(!(v->Filename[0]))
    {
      if (v->NoFiles)
        updmsg(v, GetLocalString( &li, MSG_NO_FILES_REQUIRED ), XPRU_UPLOAD);
      else
        upderr(v, GetLocalString( &li, MSG_NO_FILES_SPECIFIED ), XPRU_UPLOAD);
      done = TRUE;
    }
    /* Else use the template routines to get the first filename */
  }
  else
  {
    single = FALSE;
    fstate = (*v->io.xpr_ffirst) (v->Filename, v->io.xpr_filename);
    if (!fstate || !(v->Filename[0]))  /* There are no files found */
    {
      if (v->NoFiles) /* that's OK, we allow this kind of thing */
      {  
        updmsg(v, GetLocalString( &li, MSG_NO_FILES_REQUIRED ), XPRU_UPLOAD);
        done = TRUE;
      }
      else
      {  /* In that case, I'm leaving */
        if((!(v->io.xpr_filename) || !(v->io.xpr_filename[0]))) upderr(v,GetLocalString( &li, MSG_NO_FILES_SPECIFIED ), XPRU_UPLOAD);
          else upderr (v, GetLocalString( &li, MSG_NO_FILES_MATCH_TEMPLATE ), XPRU_UPLOAD);
        return;
      }
    }
  }

  /* If using templates, keep getting names & sending until done */
  while (!done)
  {
    if (sendone (v) == ERROR)
      return;
    if (single)
      break;
    fstate = (*v->io.xpr_fnext) (fstate, v->Filename, v->io.xpr_filename);
    done = !fstate;
  }

  /* End batch and return; if we never got started, just cancel receiver */
  if (v->Filcnt || v->NoFiles)
    saybibi (v);
  else
    canit (v);
}                                /* End of void sendbatch() */

/**********************************************************
 *      short sendone(struct Vars *v)
 *
 * Send the file named in v->Filename
 **********************************************************/
short 
sendone (struct Vars *v)
{
  struct SetupVars *sv;

#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "*** Sending %s\n", v->Filename);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif

  /* Display name of file being sent for user */
  if (v->NoMask) v->xpru.xpru_updatemask = XPRU_FILENAME;
  else v->xpru.xpru_updatemask = XPRU_UPLOAD | XPRU_FILENAME;
  v->xpru.xpru_filename = v->Filename;
  (*v->io.xpr_update) (&v->xpru);

  /* Set text/binary mode according to options before opening file */
  set_textmode (v);

  /* Open the file, if possible */
  if (!(v->File = bfopen (v, "r")))
  {
    ++v->Errcnt;
    upderr (v, GetLocalString( &li, MSG_CANT_OPEN_FILE ), XPRU_UPLOAD);
    return OK;                /* pass over it, there may be others */
  }
  ++v->Filcnt;
  getsystime (&v->Starttime);

  /* Kick off the file transfer */
  sv = (void *) v->io.xpr_data;
  switch (sendname (v))
  {
  case ERROR:
    ++v->Errcnt;
    return ERROR;
  case OK:
    bfclose (v);
    /* File sent; if option DY, delete file after sending */
    if (*sv->option_d == 'Y' && v->io.xpr_extension >= 2 && v->io.xpr_unlink)
    {
      updmsg (v, GetLocalString( &li, MSG_DELETING_FILE_AFTER_SEND ), XPRU_UPLOAD);
      (*v->io.xpr_unlink) (v->Filename);
    }
    break;
  }
  return OK;
}                                /* End of short sendone() */

/**********************************************************
 *      short sendname(struct Vars *v)
 *
 * Build file info block consisting of file name, length,
 * time, and mode
 **********************************************************/
short
sendname (struct Vars *v)
{
  struct SetupVars *sv;
  UBYTE *p, *q, buff[32];

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Initialize comm program transfer status display */
  v->Fsize = (v->io.xpr_finfo) ? (*v->io.xpr_finfo) (v->Filename, 1L) : -1;
  if (v->NoMask) 
    v->xpru.xpru_updatemask = XPRU_PROTOCOL | XPRU_FILESIZE | XPRU_MSG
      | XPRU_BLOCKS | XPRU_ERRORS | XPRU_TIMEOUTS | XPRU_BLOCKCHECK
      | XPRU_BYTES | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKSIZE;
  else
    v->xpru.xpru_updatemask = XPRU_UPLOAD | XPRU_PROTOCOL | XPRU_FILESIZE | XPRU_MSG
      | XPRU_BLOCKS | XPRU_ERRORS | XPRU_TIMEOUTS | XPRU_BLOCKCHECK
      | XPRU_BYTES | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKSIZE;

  if (v->SmallBlocks || (v->Ksize == 1024))
    v->xpru.xpru_protocol = (v->FTNmode) ? "ZedZip" : "Zmodem";
  else 
    v->xpru.xpru_protocol = (v->FTNmode) ? "ZedZap" : "SZmodem";

  if (v->dzap)
    v->xpru.xpru_protocol = "DirectZap";

  v->xpru.xpru_filesize = v->Fsize;
  v->xpru.xpru_msg = (v->Lzconv == ZCNL) ? GetLocalString( &li, MSG_SENDING_TEXT_FILE ) :
    ((v->Lzconv == ZCBIN) ? GetLocalString( &li, MSG_SENDING_BINARY_FILE ) : GetLocalString( &li, MSG_SENDING_FILE ));
  v->xpru.xpru_blocks = v->xpru.xpru_errors = v->xpru.xpru_timeouts = 0;
  v->xpru.xpru_blockcheck = v->Crc32t ? "CRC-32" : "CRC-16";
  v->xpru.xpru_bytes = v->Strtpos = 0;
  v->xpru.xpru_blocksize = v->Ksize;
  update_rate (v);
  (*v->io.xpr_update) (&v->xpru);

#ifdef KDEBUG
KPrintF("Protocol:%s    FileName:%s   FileSize:%ld\n",v->xpru.xpru_protocol,v->Filename,v->Fsize);
#endif

  sv = (void *) v->io.xpr_data;
  if (*sv->option_s == 'Y')
  {
    /* If "SY" option selected, send full path */
    strcpy (v->Pktbuf, v->Filename);
    p = v->Pktbuf + strlen (v->Pktbuf) + 1;
  }
  else
  {
    /* else extract outgoing file name without directory path */
    for (p = v->Filename, q = v->Pktbuf; *p; ++p, ++q)
      if ((*q = *p) == '/' || *q == ':')
        q = v->Pktbuf - 1;
    *q = '\0';
    p = ++q;
  }

  /* Zero out remainder of file info packet */
  memset (p, 0, sizeof (v->Pktbuf) - (p - v->Pktbuf));

  /* Store file size, timestamp, and mode in info packet */
  /*
     * XPR spec doesn't provide a way to get the file timestamp or file mode,
     * so we'll just fake it with the current time and a dummy 0.
   */
  stcl_o (buff, getsystime (NULL) + UnixTimeOffset);
  /* amiga.lib sprintf() can't do %lo format, so we do it the hard way */
  /* Yes, octal; ZModem was originally done on Unix, and they like octal there */
  xprsprintf (p, "%ld %s 0", (v->Fsize < 0) ? 0L : v->Fsize, buff);

  /* Send filename packet */
  return zsendfile (v, (short) (p - v->Pktbuf + strlen (p) + 1));
}                                /* End of short sendname() */

/**********************************************************
 *      short zsendfile(struct Vars *v, short blen)
 *
 * Send the filename packet and see if receiver will accept
 * file
 **********************************************************/
short
zsendfile (struct Vars *v, short blen)
{
  short c;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  while (TRUE)
  {
    v->Txhdr[ZF0] = v->Lzconv;      /* Text or Binary mode; from config string */
    v->Txhdr[ZF1] = LZMANAG;        /* Default file management mode */
    v->Txhdr[ZF2] = LZTRANS;        /* Default file transport mode */
    v->Txhdr[ZF3] = 0;
    zsbhdr (v, ZFILE);
    zsdata (v, blen, ZCRCW);
    sendbuf (v);
  again:
    /* Check for abort from comm program */
    if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
    {
      bfclose (v);
      return ERROR;
    }
    switch (c = zgethdr (v))
    {
    case ZRINIT:
      goto again;
    case ZCAN:
    case ZCRC:
    case RCDO:
    case TIMEOUT:
    case ZABORT:
    case ZFIN:
      upderr (v, v->Msgbuf, XPRU_UPLOAD);
      return ERROR;
    case ZSKIP:                /* Receiver doesn't want this one */
      upderr (v, GetLocalString( &li, MSG_SKIP_COMMAND_RECEIVED ), XPRU_UPLOAD);
      bfclose (v);
      return c;
    case ZRPOS:                /* Receiver wants it; this is starting position */
      bfseek (v, v->Rxpos);
      v->Strtpos = v->Txpos = v->Rxpos;
      if (v->Strtpos != 0)
        updmsg (v, GetLocalString( &li, MSG_RESUMING_SEND ), XPRU_UPLOAD);
      if (v->io.xpr_sflush)
        (*v->io.xpr_sflush) ();
      v->Modemcount = 0;
#ifdef KDEBUG
KPrintF("zgethdr starting ZRPOS: Strtpos=%ld Rxpos=%ld Txpos=%ld\n", (long) v->Strtpos, (long) v->Rxpos, (long) v->Txpos);
#endif
      return zsendfdata (v);
    }
  }
}                                /* End of short zsendfile() */

/**********************************************************
 *      short zsendfdata(struct Vars *v)
 *
 * Send the file data
 **********************************************************/
short
zsendfdata (struct Vars *v)
{
  short c, e, blklen, goodbytes = 0;
  USHORT framelen, maxblklen, goodneeded = 512;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Figure out max data packet size to send */
  if (v->FTNmode)     
  {   
    maxblklen = 
      (short) ((v->NewBaud == 0) ? (v->Baud * 8192/9600) : (v->NewBaud * 8192/9600));
 
    if (v->Ksize < maxblklen)
      maxblklen = v->Ksize;   
    if (v->Rxbuflen && (maxblklen > v->Rxbuflen))
      maxblklen = v->Rxbuflen;    
    blklen = maxblklen;   
#ifdef KDEBUG
KPrintF("FTNmode blocks\n");
#endif
  }   
  else /* not ftn mode */ 
  { 
    maxblklen = v->Ksize;
    if (v->Rxbuflen && maxblklen > v->Rxbuflen)
      maxblklen = v->Rxbuflen;
    blklen = (v->NewBaud == 0) ? ((v->Baud < 1200) ? 256 : v->Ksize) : ((v->NewBaud < 1200) ? 256 : v->Ksize);
    if (blklen > maxblklen)
      blklen = maxblklen;
  }   
#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "Rxbuflen=%ld blklen=%ld\n",
     (long) v->Rxbuflen, (long) blklen);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif
#ifdef KDEBUG
KPrintF("Rxbuflen=%ld  maxblklen=%ld  blklen=%ld\n",
    (long) v->Rxbuflen, (long) maxblklen, (long) blklen);
#endif

  /* If an interruption happened, handle it; else keep sending data */
somemore:
  while (char_avail (v))
  {
    /* Check for another incoming packet while discarding line noise */
    switch (readock (v, 1))
    {
      case CAN:
      case RCDO:
      case ZPAD:
        break;
      default:
        continue;
    }
  waitack:
#ifdef DEBUGLOG
    dlog (v, "--- At waitack\n");
    D (DEBUGINFO);
#endif
    switch (c = getinsync (v))
    {
      default:
        upderr (v, GetLocalString( &li, MSG_TRANSFER_CANCELLED ), XPRU_UPLOAD);
        bfclose (v);
        return ERROR;
      case ZSKIP:                /* Receiver changed its mind and wants to skip the file */
        return c;
      case ZACK:                /* ACK at end of frame; resume sending data */
        break;
      case ZRPOS:                /* An error; resend data from last good point */
#ifdef KDEBUG
KPrintF("zgetinsync returned ZRPOS: Strtpos=%ld Rxpos=%ld Txpos=%ld\n", (long) v->Strtpos, (long) v->Rxpos, (long) v->Txpos);
#endif
        blklen >>= 2;
        if (blklen < MINBLOCK)
          blklen = MINBLOCK;
        if (goodneeded < MAXGOODNEEDED)
          goodneeded <<= 1;
#ifdef KDEBUG
KPrintF("bumping goodneeded/blklen: maxblklen=%ld  goodneeded=%ld\nzgetinsync ZRPOS:blklen=%ld  goodbytes=%ld\n",
        (long) maxblklen, (long) goodneeded, (long) blklen, (long) goodbytes);
#endif
        if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORS;
        else v->xpru.xpru_updatemask = XPRU_UPLOAD | XPRU_ERRORS;
        ++v->xpru.xpru_errors;
        (*v->io.xpr_update) (&v->xpru);
        break;
      case ZRINIT:
        updmsg (v, GetLocalString( &li, MSG_DONE ), XPRU_UPLOAD);
        return OK;
    }
  } /* while charavail */

  /* Transmit ZDATA frame header */
  framelen = v->Rxbuflen;
  stohdr (v, v->Txpos);
  zsbhdr (v, ZDATA);

  /* Keep sending data packets until finished or interrupted */
  do
  {
    /* Read next chunk of file data */
    c = bfread (v, v->Pktbuf, (long) blklen);

    /* Figure out how to handle this data packet */
    if (c < blklen)
      e = ZCRCE;                /* If end of file, this is last data packet */
    else if (v->Rxbuflen && (framelen -= c) <= 0)
      e = ZCRCW;                /* If end of frame, ask for ACK */
    else
      e = ZCRCG;                /* Else tell receiver to expect more data packets */

    zsdata (v, c, e);                /* Send the packet */
    sendbuf (v);

    /* Update comm program status display */
    if (v->NoMask) 
      v->xpru.xpru_updatemask = XPRU_BLOCKS | XPRU_BLOCKSIZE | XPRU_BYTES
        | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE
        | XPRU_BLOCKCHECK;
    else 
      v->xpru.xpru_updatemask = XPRU_UPLOAD | XPRU_BLOCKS | XPRU_BLOCKSIZE | XPRU_BYTES
        | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE
        | XPRU_BLOCKCHECK;
    ++v->xpru.xpru_blocks;
    v->xpru.xpru_blocksize = c;
    v->xpru.xpru_blockcheck = v->Crc32t ? "CRC-32" : "CRC-16";
    v->xpru.xpru_bytes = v->Txpos += c;
    update_rate (v);
    (*v->io.xpr_update) (&v->xpru);

#ifdef KDEBUG
KPrintF("sent: maxblklen=%ld  goodneeded=%ld Sent=%ld   Pos:%ld\n         blklen=%ld            goodbytes=%ld\n",
        (long) maxblklen, (long) goodneeded, (long) c, (long) v->Txpos, (long) blklen, (long) goodbytes);
#endif
    /*
       * If we've been sending smaller than normal packets, see if it's
       * time to bump the packet size up a notch yet
     */
    if (blklen < maxblklen && (goodbytes += c) >= goodneeded)
    {
      blklen <<= 1;
      if (blklen > maxblklen)
        blklen = maxblklen;
#ifdef KDEBUG
KPrintF("bump: maxblklen=%ld  goodneeded=%ld Sent=%ld   Pos:%ld\nbump to: blklen=%ld            goodbytes=%ld\n",
        (long) maxblklen, (long) goodneeded, (long) c, (long) v->Txpos, (long) blklen, (long) goodbytes);
#endif
      goodbytes = 0;
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "Bumping packet size to %ld at %ld\n",
                  (long) blklen, v->Txpos);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
    }

    /* Give comm program its timeslice if it needs one */
    if (v->io.xpr_chkmisc)
      (*v->io.xpr_chkmisc) ();
    /* Check for abort from comm program */
    if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
      goto aborted;
    /* If this was last packet in frame, go wait for ACK from receiver */
    if (e == ZCRCW)
    {
#ifdef KDEBUG
KPrintF("last packet in frame, waiting for ACK\n");
#endif
      goto waitack;
    }
    /*
       * Check if receiver trying to interrupt us; look for incoming packet
       * while discarding line noise
     */
#ifdef KDEBUG
KPrintF("Looking for interrupt ---");
#endif
    while (char_avail (v))
    {
#ifdef KDEBUG
KPrintF("char_avail\n");
#endif

      switch (readock (v, 1))
      {
      case CAN:
      case RCDO:
      case ZPAD:
        /* Interruption detected; stop sending and process complaint */
#ifdef KDEBUG
KPrintF("Got ZPAD, send interrupted\n");
#endif
#ifdef DEBUGLOG
        dlog (v, "--- Interrupted send\n");
        D (DEBUGINFO);
#endif
        zsdata (v, 0, ZCRCE);
        sendbuf (v);
        goto waitack;
      }
    }
#ifdef KDEBUG
KPrintF("No char_avail\n");
#endif

  } while (e == ZCRCG);                /* If no interruption, keep sending data packets */

  /* Done sending file data; send EOF and wait for receiver to acknowledge */
  while (TRUE)
  {
    updmsg (v, GetLocalString( &li, MSG_SENDING_EOF ), XPRU_UPLOAD);
    stohdr (v, v->Txpos);
    zsbhdr (v, ZEOF);
    sendbuf (v);
    switch (c = getinsync (v))
    {
    case ZACK:
      continue;
    case ZRPOS:
      goto somemore;
    case ZRINIT:
      updmsg (v, GetLocalString( &li, MSG_EOF_ACKNOWLEDGED ), XPRU_UPLOAD);
      ++v->Starttime.tv_secs;
      update_rate (v);
      v->xpru.xpru_updatemask = XPRU_UPLOAD | XPRU_EXPECTTIME | 
                                XPRU_ELAPSEDTIME | XPRU_DATARATE;
      (*v->io.xpr_update) (&v->xpru);
      return OK;
    case ZSKIP:
      return c;
    default:
  aborted:
      upderr (v, GetLocalString( &li, MSG_TRANSFER_CANCELLED ), XPRU_UPLOAD);
      bfclose (v);
      return ERROR;
    }
  }
}                                /* End of short zsendfdata() */

/**********************************************************
 *      short getinsync(struct Vars *v)
 *
 * Respond to receiver's complaint, get back in sync with
 * receiver
 **********************************************************/
short
getinsync (struct Vars *v)
{
  short c;

  while (TRUE)
  {
#ifdef DEBUGLOG
    dlog (v, "--- At getinsync\n");
    D (DEBUGINFO);
#endif
    c = zgethdr (v);
    if (v->io.xpr_sflush)
      (*v->io.xpr_sflush) ();
    v->Modemcount = 0;
    switch (c)
    {
    case ZCAN:
    case ZABORT:
    case ZFIN:
    case RCDO:
    case TIMEOUT:
      upderr (v, v->Msgbuf, XPRU_UPLOAD);
      return ERROR;
    case ZRPOS:
#ifdef KDEBUG
KPrintF("in GIS ZRPOS:Strtpos=%ld Rxpos=%ld Txpos=%ld\n",
   (long) v->Strtpos, (long) v->Rxpos, (long) v->Txpos);
#endif
      bfseek (v, v->Rxpos);
      v->Txpos = v->Rxpos;
      xprsprintf (v->Msgbuf, "%s %ld", GetLocalString( &li, MSG_RESENDING_FROM ), v->Txpos);
      upderr (v, v->Msgbuf, XPRU_UPLOAD);
#ifdef KDEBUG
KPrintF("repositioning:Strtpos=%ld Rxpos=%ld Txpos=%ld\n",
   (long) v->Strtpos, (long) v->Rxpos, (long) v->Txpos);
#endif
      return c;
    case ZSKIP:
      upderr (v, GetLocalString( &li, MSG_SKIP_COMMAND_RECEIVED ), XPRU_UPLOAD);
      /* fallthrough... */
    case ZRINIT:
      bfclose (v);
      /* fallthrough... */
    case ZACK:
      return c;
    default:
      zsbhdr (v, ZNAK);
      sendbuf (v);
      continue;
    }
  }
}                                /* End of short getinsync() */

/**********************************************************
 *      void saybibi(struct Vars *v)
 *
 * End of batch transmission; disengage cleanly from receiver
 **********************************************************/
void
saybibi (struct Vars *v)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  while (TRUE)
  {
    stohdr (v, 0L);
    zsbhdr (v, ZFIN);
    sendbuf (v);
    switch (zgethdr (v))
    {
    case ZFIN:
      sendline (v, 'O');
      sendline (v, 'O');
      sendbuf (v);
      /* fallthrough... */
    case ZCAN:
    case RCDO:
    case TIMEOUT:
      return;
    }
  }
}                                /* End of void saybibi() */
/* End of Send.c source */
