/**********************************************************************
 * Receive.c:  File reception routines for xprzmodem.library;
 * Version 2.10, 12 February 1991, by Rick Huebner.
 * Based closely on Chuck Forsberg's rz.c example ZModem code,
 * but too pervasively modified to even think of detailing the changes.
 * Released to the Public Domain; do as you like with this code.
 *
 * Version 2.50, 15 November 1991, CRC-32 additions by William M. Perkins.
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
 *      long XProtocolReceive(struct XPR_IO *xio)
 *
 * Main file reception routine; called by comm program
 **********************************************************/
long __saveds __asm
XProtocolReceive (register __a0 struct XPR_IO *xio)
{
  struct SetupVars *sv;
  struct Vars *v;
  UBYTE err = FALSE;

  /* Perform common setup and initializations */
  if (!(v = setup (xio)))
    return XPRS_FAILURE;

  v->Tryzhdrtype = ZRINIT;
  v->Rxtimeout = v->FTNmode ? 600 : 100;

#ifdef KDEBUG
KPrintF("\nXProtocolReceive\n");
KPrintF("XprBaud:%ld  UserBaud:%ld\n",v->Baud,v->NewBaud);
KPrintF("FtnMode:%s   DirectZap:%s   Nofiles:%s   SmallBlocks:%s\n",
  (v->FTNmode ? "TRUE" : "FALSE"),
  (v->dzap ? "TRUE" : "FALSE"),
  (v->NoFiles ? "TRUE" : "FALSE"),
  (v->SmallBlocks ? "TRUE" : "FALSE"));
KPrintF("Rxtimeout:%ld\n",v->Rxtimeout);
#endif

  sv = (void *) v->io.xpr_data;
  if (sv->bufpos)
    {
      v->Modemchar = v->Modembuf;
      if (sv->buflen > sizeof (v->Modembuf))
        sv->buflen = sizeof (v->Modembuf);
      memcpy (v->Modembuf, sv->bufpos, sv->buflen);
      v->Modemcount = sv->buflen;
    }

  /* first clear the inbound buffer */
  if (v->FTNmode && v->io.xpr_sflush)   
  {       
#ifdef KDEBUG 
KPrintF("Flushing\n");  
#endif  
    (*v->io.xpr_sflush) ();     
  }   

  /* Transfer the files */
  if (rcvbatch (v) == ERROR)
  {
    upderr (v, GetLocalString( &li, MSG_DOWNLOAD_USER_ERROR ), XPRU_DNLOAD);
    err = TRUE;
  }
  else
  {
    updmsg (v, GetLocalString( &li, MSG_RECV_DONE ), XPRU_DNLOAD);
  }
  /* Clean up and return */
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
KPrintF("XProtcolReceive returning:%s\n",err ? "XPRS_FAILURE" : "XPRS_SUCCESS");
#endif
  return (err) ? XPRS_FAILURE : XPRS_SUCCESS;
} /* End of long XProtocolReceive() */

/**********************************************************
 *      short rcvbatch(struct Vars *v)
 *
 * Start the batch transfer
 **********************************************************/
short
rcvbatch (struct Vars *v)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif
ignore_zcan:
  switch (tryz (v))
  {
    case ZCOMPL:
      return OK;
    case ZFILE:
      if (rzfiles (v) == OK)
        return OK;
/**/    case ZFIN:
      if (v->NoFiles)
        return OK;
    case ZCAN:
      if (v->FTNmode)
        goto ignore_zcan;
    }

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif
#ifdef KDEBUG
KPrintF("rcvbatch canit ERROR\n");
#endif
  canit (v);

  return ERROR;
}                                /* End of short rcvbatch() */

/**********************************************************
 *      short tryz(struct Vars *v)
 *
 * Negotiate with sender to start a file transfer
 **********************************************************/
short
tryz (struct Vars *v)
{
  short n, errors = 0;
/*  short flushs = 0; */

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  for (n = v->ErrorLimit; --n >= 0;)
  {
      /* Set max frame length and capability flags */
      stohdr (v, (long) v->Tframlen);
      v->Txhdr[ZF0] = CANFC32 | CANFDX | CANOVIO;
      zshhdr (v, v->Tryzhdrtype);
      sendbuf (v);

    again:
      /* Check for abort from comm program */
      if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
      {
#ifdef KDEBUG
KPrintF("Host aborted us!!!\n");
#endif
        return ERROR;
      }
      switch (zgethdr (v))
      {
/*
        case ZRQINIT:
        case ZRINIT:
          goto again;
*/
        case ZFIN:                /* Sender has ended batch */
          ackbibi (v);
          return ZCOMPL;
        case ZFILE:                   /* File name and info packet */
          v->Zconv = v->Rxhdr[ZF0];   /* Suggested txt mode; ZCNL = text, */
                                      /* ZCBIN = binary, 0 = don't know. */
          v->Zmanag = v->Rxhdr[ZF1];  /* Suggested file management mode. */
          v->Ztrans = v->Rxhdr[ZF2];  /* Suggested file transport mode. */
          v->Tryzhdrtype = ZRINIT;
/*          if (zrdata (v, v->Pktbuf, KSIZE) == GOTCRCW)    */
          if (zrdata (v, v->Pktbuf, v->Ksize) == GOTCRCW) 
            return ZFILE;
          zshhdr (v, ZNAK);         /* Packet mangled, ask for retry */
          sendbuf (v);
          goto again;
        case ZSINIT:                /* Special attention-grabbing string to use to */
                                    /* interrupt sender */
          if (zrdata (v, v->Attn, ZATTNLEN) == GOTCRCW)
            zshhdr (v, ZACK);
          else
            zshhdr (v, ZNAK);
          sendbuf (v);
          goto again;
        case ZFREECNT:                /* Sender wants to know how much room we've got */
          stohdr (v, getfree ());
          zshhdr (v, ZACK);
          sendbuf (v);
          goto again;
        case ZCOMMAND:                /* Sender wants us to do remote commands, */
                                      /* but we don't do requests. */
/*          if (zrdata (v, v->Pktbuf, KSIZE) == GOTCRCW)    */
          if (zrdata (v, v->Pktbuf, v->Ksize) == GOTCRCW) 
          {
            xprsprintf (v->Msgbuf, "%s: %s", GetLocalString( &li, MSG_IGNORING_COMMAND), v->Pktbuf);
            upderr (v, v->Msgbuf, XPRU_DNLOAD);    /* Ignore and report all uploaded commands */
            stohdr (v, 0L);           /* whilst telling sender they worked; */
            do
            {
              zshhdr (v, ZCOMPL);    /* paranoia can be good for you... */
              sendbuf (v);
            }
            while (++errors < v->ErrorLimit && zgethdr (v) != ZFIN);
              ackbibi (v);
            return ZCOMPL;
          }
          else
            zshhdr (v, ZNAK);
          sendbuf (v);
          goto again;
        case ZCOMPL:
          goto again;
        case ZCAN:
#ifdef KDEBUG
KPrintF("tryz: Got a ZCAN\n");
#endif
        case RCDO:
          upderr (v, v->Msgbuf, XPRU_DNLOAD);
          return ERROR;
      }
  }
#ifdef KDEBUG
KPrintF("tryz returning ERROR ,error limit:  %ld exceeded\n",v->ErrorLimit);
#endif
  return ERROR;
}                                /* End of short tryz() */

/**********************************************************
 *      short rzfiles(struct Vars *v)
 *
 * Receive a batch of files
 **********************************************************/
short
rzfiles (struct Vars *v)
{
  struct SetupVars *sv;
  short c;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Keep receiving files until end of batch or error */
  while (TRUE)
    {
      if (v->FTNmode)
        v->Filename[0]='\0';

      switch (c = rzfile (v))
        {
/**/        case ZFIN:
            return OK;

        case ZEOF:
        case ZSKIP:
          if (v->FTNmode && strlen(v->Filename))
          {
#ifdef KDEBUG
KPrintF("Returning XPRS_SUCCESS for ZEOF/ZSKIP in FTNmode\n");
#endif
            updstatus(v,v->Filename,XPRS_SUCCESS, XPRU_DNLOAD);
          }
          switch (tryz (v))
            {
            case ZCOMPL:
              return OK;
            case ZFILE:
              break;
            default:
              return ERROR;
            }
          break;
        default:
          bfclose (v);
          if (v->FTNmode && strlen(v->Filename))
          {
#ifdef KDEBUG
KPrintF("Returning XPRS_FAILURE in FTNmode\n");
#endif
            updstatus (v,v->Filename,XPRS_FAILURE, XPRU_DNLOAD);  /* Tell the system it failed! */
          }

          sv = (void *) v->io.xpr_data;
          if (*sv->option_k == 'N' && v->io.xpr_extension >= 2
              && v->io.xpr_unlink)
            {
              updmsg (v, GetLocalString( &li, MSG_DELETING_RECEIVED_FILE ), XPRU_DNLOAD);
              (*v->io.xpr_unlink) (v->Filename);
            }
          else
            updmsg (v, GetLocalString( &li, MSG_KEEPING_RECEIVED_FILE), XPRU_DNLOAD);
          return c;
        }
    }
}                                /* End of short rzfiles() */

/**********************************************************
 *      short rzfile(struct Vars *v)
 *
 * Receive one file; file name packet already read into
 * Pktbuf by tryz()
 **********************************************************/
short
rzfile (struct Vars *v)
{
  short c, n;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /*
     * Process file name packet; either open file and prepare to receive,
     * or tell us to skip this one.
   */
  if (procheader (v) == ERROR)
    return v->Tryzhdrtype = ZSKIP;

  n = v->ErrorLimit;
  v->Rxbytes = v->Strtpos;
  v->Eofseen = FALSE;

  /* Receive ZDATA frames until finished */
  while (TRUE)
    {
      stohdr (v, v->Rxbytes);        /* Tell sender where to start frame */
      zshhdr (v, ZRPOS);
      sendbuf (v);
    nxthdr:
      /* Check for abort from comm program */
      if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
      {
#ifdef KDEBUG
KPrintF("Host aborted us!!!\n");
#endif
        return ERROR;
      }
      switch (c = zgethdr (v))        /* Wait for frame header */
        {
        default:
#ifdef DEBUGLOG
          xprsprintf (v->Msgbuf, "rzfile: zgethdr returned %ld\n", (long) c);
          dlog (v, v->Msgbuf);
          D (DEBUGINFO);

#endif
          return ERROR;
        case ZNAK:
        case TIMEOUT:
          if (--n < 0)
            return ERROR;
#ifdef DEBUGLOG
          dlog (v, "rzfile: zgethdr NAK/Timeout\n");
          D (DEBUGINFO);
#endif
          if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORMSG | XPRU_TIMEOUTS;
          else v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_ERRORMSG | XPRU_TIMEOUTS;
          xprsprintf (strchr (v->Msgbuf, '\0'), "@ %ld; %ld %s",
                      v->Rxbytes, (long) n, GetLocalString( &li, MSG_RETRIES_LEFT ));
          v->xpru.xpru_errormsg = (char *) v->Msgbuf;
          ++v->xpru.xpru_timeouts;
          (*v->io.xpr_update) (&v->xpru);
          continue;
        case ZFILE:                /* Sender didn't see our ZRPOS yet; try again */
/*          zrdata (v, v->Pktbuf, KSIZE);   */
          zrdata (v, v->Pktbuf, v->Ksize);  
                /* Read and discard redundant */
          continue;                /* filename packet */
        case ZEOF:                /* End of file data */
          if (v->Rxpos != v->Rxbytes)        /* We aren't in sync; go back */
            {
              xprsprintf (v->Msgbuf, GetLocalString( &li, MSG_BAD_EOF ),
                          v->Rxbytes, v->Rxpos);
              upderr (v, v->Msgbuf, XPRU_DNLOAD);
              continue;
            }
          bfclose (v);                /* All done; close file */
#ifdef DEBUGLOG
          dlog (v, "rzfile: EOF\n");
          D (DEBUGINFO);
#endif
          updmsg (v, GetLocalString( &li, MSG_EOF_RECEIVED ), XPRU_DNLOAD);
          return c;
        case ERROR:                /* Too much garbage while waiting for frame header */
          if (--n < 0)
            return ERROR;
#ifdef DEBUGLOG
          dlog (v, "rzfile: zgethdr garbage overflow\n");
          D (DEBUGINFO);
#endif
          if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORMSG | XPRU_ERRORS;
          else v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_ERRORMSG | XPRU_ERRORS;
          xprsprintf (strchr (v->Msgbuf, '\0'), "@ %ld; %ld %s",
                      v->Rxbytes, (long) n, GetLocalString( &li, MSG_RETRIES_LEFT ));
          v->xpru.xpru_errormsg = (char *) v->Msgbuf;
          ++v->xpru.xpru_errors;
          (*v->io.xpr_update) (&v->xpru);
          zmputs (v, v->Attn);
          continue;
        case ZDATA:                /* More file data packets forthcoming */
            if ((v->Rxpos > v->Rxbytes) || ((v->Rxpos + 2*(v->Rxcount)) < v->Rxbytes))
/*          if (v->Rxpos != v->Rxbytes)     */
            {    /* We aren't in sync; go back */
              if (--n < 0)
                return ERROR;
              if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORMSG | XPRU_ERRORS;
              else v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_ERRORMSG | XPRU_ERRORS;
              xprsprintf (v->Msgbuf, GetLocalString( &li, MSG_DATA_AT_BAD_POSITION ),
                          v->Rxbytes, v->Rxpos);
              v->xpru.xpru_errormsg = (char *) v->Msgbuf;
              ++v->xpru.xpru_errors;
              (*v->io.xpr_update) (&v->xpru);
              zmputs (v, v->Attn);
              continue;
            }
          /* Receive file data packet(s) */
        moredata:
          /* Give comm program its timeslice if it needs one */
          if (v->io.xpr_chkmisc)
            (*v->io.xpr_chkmisc) ();
          /* Check for abort from comm program */
          if (v->io.xpr_chkabort && (*v->io.xpr_chkabort) ())
          {
#ifdef KDEBUG
KPrintF("Host aborted us!!!\n");
#endif
            goto aborted;
          }
/*          switch (c = zrdata (v, v->Pktbuf, KSIZE))   */
          switch (c = zrdata (v, v->Pktbuf, v->Ksize))  
            {
            case ZCAN:
#ifdef KDEBUG
KPrintF("rzfile: zdata Got a ZCAN\n");
#endif
            case RCDO:
            aborted:
#ifdef DEBUGLOG
              dlog (v, "rzfile: zrdata returned CAN\n");
              D (DEBUGINFO);
#endif
              upderr (v, GetLocalString( &li, MSG_TRANSFER_CANCELLED ), XPRU_DNLOAD);
              return ERROR;
            case ERROR:        /* CRC error or packet too long */
              if (--n < 0)
                return ERROR;
#ifdef DEBUGLOG
              dlog (v, "rzfile: zrdata returned error\n");
              D (DEBUGINFO);
#endif
              if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORMSG | XPRU_ERRORS;
              else v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_ERRORMSG | XPRU_ERRORS;
              xprsprintf (strchr (v->Msgbuf, '\0'), "@ %ld; %ld %s",
                          v->Rxbytes, (long) n, GetLocalString( &li, MSG_RETRIES_LEFT ));
              v->xpru.xpru_errormsg = (char *) v->Msgbuf;
              ++v->xpru.xpru_errors;
              (*v->io.xpr_update) (&v->xpru);
#ifdef DEBUGLOG
              dlog (v, v->Msgbuf);
              dlog (v, "\n");
              D (DEBUGINFO);
#endif
              zmputs (v, v->Attn);
              continue;
            case TIMEOUT:
              if (--n < 0)
                return ERROR;
#ifdef DEBUGLOG
              dlog (v, "rzfile: zrdata returned timeout\n");
              D (DEBUGINFO);
#endif
              if (v->NoMask) v->xpru.xpru_updatemask = XPRU_ERRORMSG | XPRU_TIMEOUTS;
              else v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_ERRORMSG | XPRU_TIMEOUTS;
              xprsprintf (strchr (v->Msgbuf, '\0'), "@ %ld; %ld %s",
                          v->Rxbytes, (long) n, GetLocalString( &li, MSG_RETRIES_LEFT ));
              v->xpru.xpru_errormsg = (char *) v->Msgbuf;
              ++v->xpru.xpru_timeouts;
              (*v->io.xpr_update) (&v->xpru);
#ifdef DEBUGLOG
              dlog (v, v->Msgbuf);
              dlog (v, "\n");
              D (DEBUGINFO);
#endif
              continue;
            case GOTCRCW:        /* Sender says it's waiting for an ACK */
              n = v->ErrorLimit;
              if (putsec (v) == ERROR)
                return ERROR;
              stohdr (v, v->Rxbytes);
              zshhdr (v, ZACK);
              sendbuf (v);
              goto nxthdr;
            case GOTCRCQ:        /* Sender says it's not waiting, */
              /* but ACK anyway (rarely used) */
              n = v->ErrorLimit;
              if (putsec (v) == ERROR)
                return ERROR;
              stohdr (v, v->Rxbytes);
              zshhdr (v, ZACK);
              sendbuf (v);
              goto moredata;
            case GOTCRCG:        /* Sender says keep receiving, there's more coming */
              n = v->ErrorLimit;
              if (putsec (v) == ERROR)
                return ERROR;
              goto moredata;
            case GOTCRCE:        /* Sender says this is the last packet */
              n = v->ErrorLimit;
              if (putsec (v) == ERROR)
                return ERROR;
              goto nxthdr;
            }
        }
    }
}                                /* End of short rzfile() */

/**********************************************************
 *      short procheader(struct Vars *v)
 *
 * Process file name & info packet; either open file and
 * prepare to receive, or return ERROR if we should skip
 * this one for some reason
 **********************************************************/
short
procheader (struct Vars *v)
{
  struct SetupVars *sv;
  UBYTE *p, *openmode, buff[PATHLEN];
  long n;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  openmode = "w";
  v->Strtpos = 0;

  /* Extract expected filesize from file info packet, if given */
  v->Fsize = -1;
  p = strchr (v->Pktbuf, '\0') + 1;
  if (*p)
    v->Fsize = atol (p);
  /*
     * Make sure we have room for file; skip it if not.
     * Commented out for now, since getfree() isn't implemented yet.
     if (v->Fsize > getfree())
     {
     xprsprintf(v->Msgbuf, GetLocalString( &li, MSG_INSUFFICIENT_DISK_SPACE ),
     v->Fsize, getfree());
     upderr(v, v->Msgbuf, XPRU_DNLOAD);
     v->Noroom = TRUE;
     return ERROR;
     }
   */

  /* If option RY set, use full received file path */
  sv = (void *) v->io.xpr_data;
  if (*sv->option_r == 'Y')
    strcpy (v->Filename, v->Pktbuf);
  else
    {
      /* else use the default directory path specified in the setup options */
      strcpy (v->Filename, sv->option_p);
      p = v->Filename + strlen (v->Filename) - 1;
      if (p >= v->Filename && *p != '/' && *p != ':')
        *++p = '/';
      *++p = '\0';
      /*
         * Append the filename from the file info packet; ignore anything before
         * last /, \, or : in filename (received directory path is ignored)
       */
      p = strchr (v->Pktbuf, '\0');        /* start at end and scan back */
      /* to start of name */
      while (p >= v->Pktbuf && *p != '/' && *p != '\\' && *p != ':')
        --p;
      strcat (v->Filename, ++p);
    }

  /* Display name of file being received for user */
  v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_FILENAME;
  v->xpru.xpru_filename = (char *) v->Filename;
  (*v->io.xpr_update) (&v->xpru);

  /*
     * If a file with this name already exists, handle in
     * accordance with O option
   */
  if (exist (v))
    {
      switch (*sv->option_o)
        {
        case 'N':                /* Don't overwrite; change name to prevent collision */
          strcpy (buff, v->Filename);
          strcat (v->Filename, ".dup");
          n = 2;
          while (exist (v))
            {
              xprsprintf (v->Filename, "%s.dup%ld", buff, n);
              ++n;
            }
          /* Update filename display to show new name */
          (*v->io.xpr_update) (&v->xpru);
          break;
        case 'R':                /* Resume transfer from current end of file */
          openmode = "a";
          v->Strtpos = (*v->io.xpr_finfo) (v->Filename, 1L);
          break;
        case 'S':                /* Skip it */
          upderr (v, GetLocalString( &li, MSG_FILE_EXISTS ), XPRU_DNLOAD);
          return ERROR;
          /* Else 'Y', go ahead and overwrite it (openmode = w) */
        }
    }

  /* Set text/binary mode according to options before opening file */
  set_textmode (v);

  /*
     * Figure out file translation mode to use; either binary (verbatim
     * transfer) or ASCII (perform end-of-line conversions).  If user has
     * specified a mode (TY or TN), that's what we use.  If user says use
     * sender's suggestion (T?), set mode according to Zconv flag.  If neither
     * side specifies, default to binary mode.
   */
  v->Thisbinary = v->Rxbinary || !v->Rxascii;
  if (!v->Rxbinary && v->Zconv == ZCNL)
    v->Thisbinary = FALSE;
  if (!v->Rxascii && v->Zconv == ZCBIN)
    v->Thisbinary = TRUE;

  /* Open the file (finally) */
  if (!(v->File = bfopen (v, openmode)))
    {
      ++v->Errcnt;
      upderr (v, GetLocalString( &li, MSG_CANT_OPEN_FILE ), XPRU_DNLOAD);
      return ERROR;
    }
  getsystime (&v->Starttime);

  /* Initialize comm program transfer status display */
  if (v->NoMask) 
    v->xpru.xpru_updatemask = XPRU_PROTOCOL | XPRU_FILESIZE | XPRU_MSG
      | XPRU_BLOCKS | XPRU_ERRORS | XPRU_TIMEOUTS | XPRU_BLOCKCHECK
      | XPRU_BYTES | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKSIZE;
  else
    v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_PROTOCOL | XPRU_FILESIZE | XPRU_MSG
      | XPRU_BLOCKS | XPRU_ERRORS | XPRU_TIMEOUTS | XPRU_BLOCKCHECK
      | XPRU_BYTES | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKSIZE;

  if (v->SmallBlocks || (v->Ksize == 1024))
    v->xpru.xpru_protocol = (v->FTNmode) ? "ZedZip" : "Zmodem";
  else 
    v->xpru.xpru_protocol = (v->FTNmode) ? "ZedZap" : "SZmodem";

  if (v->dzap)
    v->xpru.xpru_protocol = "DirectZap";

  v->xpru.xpru_filesize = v->Fsize;
  v->xpru.xpru_msg = (v->Thisbinary) ? GetLocalString( &li, MSG_RECEIVE_BINARY )
    : GetLocalString( &li, MSG_RECEIVE_TEXT );
  v->xpru.xpru_blocks = v->xpru.xpru_errors = v->xpru.xpru_timeouts = 0;
  v->xpru.xpru_blockcheck = v->Crc32 ? "CRC-32" : "CRC-16";
  v->xpru.xpru_bytes = v->Strtpos;
  v->xpru.xpru_blocksize = v->Ksize;  
  update_rate (v);
  (*v->io.xpr_update) (&v->xpru);
  if (v->Strtpos != 0)
    updmsg (v, GetLocalString( &li, MSG_RESUMING_RECV ), XPRU_DNLOAD);

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  return OK;
}                                /* End of short procheader() */

/**********************************************************
 *      short putsec(struct Vars *v)
 *
 * Writes the received file data to the output file.
 * If in ASCII mode, stops writing at first ^Z, and converts all
 * \r\n pairs or solo \r's to \n's.
 **********************************************************/
short
putsec (struct Vars *v)
{
  static char nl = '\n';
  UBYTE *p;
  short n;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* If in binary mode, write it out verbatim */
  if (v->Thisbinary)
    {
      if (bfwrite (v, v->Pktbuf, (long) v->Rxcount) != v->Rxcount)
        goto diskfull;
      /* If in text mode, perform end-of-line cleanup */
    }
  else
    {
      if (v->Eofseen)
        return OK;
      for (p = v->Pktbuf, n = v->Rxcount; --n >= 0; ++p)
        {
          if (*p == CPMEOF)
            {
              v->Eofseen = TRUE;
              return OK;
            }
          else if (*p != '\n' && v->Lastsent == '\r')
            {
              if (bfwrite (v, &nl, 1L) != 1)
                goto diskfull;
            }
          if (*p != '\r' && bfwrite (v, p, 1L) != 1)
            goto diskfull;
          v->Lastsent = *p;
        }
    }

  /* Update comm program status display */
  if (v->NoMask) 
    v->xpru.xpru_updatemask = XPRU_BLOCKS | XPRU_BLOCKSIZE | XPRU_BYTES
      | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKCHECK;
  else 
    v->xpru.xpru_updatemask = XPRU_DNLOAD | XPRU_BLOCKS | XPRU_BLOCKSIZE | XPRU_BYTES
      | XPRU_EXPECTTIME | XPRU_ELAPSEDTIME | XPRU_DATARATE | XPRU_BLOCKCHECK;
  ++v->xpru.xpru_blocks;
  v->xpru.xpru_blocksize = v->Rxcount;
  v->xpru.xpru_bytes = v->Rxbytes += v->Rxcount;
  v->xpru.xpru_blockcheck = v->Crc32 ? "CRC-32" : "CRC-16";
  update_rate (v);
  (*v->io.xpr_update) (&v->xpru);

  return OK;

diskfull:
  upderr (v, GetLocalString( &li, MSG_ERROR_WRITING_FILE ), XPRU_DNLOAD);
  v->Noroom = TRUE;
  return ERROR;
}                                /* End of short putsec() */

/**********************************************************
 *      void ackbibi(struct Vars *v)
 *
 * End of batch transmission; disengage cleanly from sender
 **********************************************************/
void
ackbibi (struct Vars *v)
{
  short n;

#ifdef DEBUGLOG
  dlog (v, "ackbibi:\n");
  D (DEBUGINFO);
#endif
  stohdr (v, 0L);
  if (v->FTNmode)
  {
    zshhdr (v, ZFIN);
    sendbuf (v);
    return;
  }
  else 
  {
    for (n = 4; --n;)
      {
        zshhdr (v, ZFIN);
        sendbuf (v);
        switch (readock (v, 100))
          {
          case 'O':
            readock (v, 1);        /* Discard 2nd 'O' */
          case TIMEOUT:
          case RCDO:
            return;
          }
      }
  }
}                                /* End of void ackbibi() */
/* End of Receive.c source */
