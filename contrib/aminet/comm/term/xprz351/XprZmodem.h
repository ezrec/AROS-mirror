/*
 *  xprzmodem.h: Definitions for xprzmodem.library and xprzedzap.library;
 *  Version 2.0, 28 October 1989, by Rick Huebner.
 *  Released to the Public Domain; do as you like with this code.
 *
 *  Version 2.63, 30 July 1993 build in locale, by Rainer Hess
 *
 *  Version 2.64, 3 Aug 1993 global variable Blocksize, now in
 *                struct Vars, by Rainer Hess
 *  Version 3.2   11 Nov 1994 
 *                restored FTN support
 *                reduced modem output buffer for faster error receovery
 *                increased pktbuf size
 *                added NewBaud, link bps
 *                added booleans FTNmode, dzap, Nofiles, Smallblocks
 *                added options c,z,q,n
 *                added prototype for KPrintF when KDEBUG is true
 *                by Robert Williamson
 */

/* #define DEBUGLOG 1 */

/* Return codes */
#define OK        0
#define ERROR   (-1)
#define TIMEOUT (-2)
#define RCDO    (-3)

/* Relevant control characters */
#define CR        ('M' & 0x1F)        /* ^M */
#define DLE       ('P' & 0x1F)        /* ^P */
#define XON       ('Q' & 0x1F)        /* ^Q */
#define XOFF      ('S' & 0x1F)        /* ^S */
#define CAN       ('X' & 0x1F)        /* ^X */
#define CPMEOF    ('Z' & 0x1F)        /* ^Z */

/* Misc. program constants */
#define LZMANAG          0        /* Default ZMODEM file management mode */
#define LZTRANS          0        /* Default ZMODEM file transport mode */

#define PATHLEN        256        /* What's the max legal path size? */
#define CONFIGLEN       32        /* Max length of transfer options string */

#define KSIZE         8192        /* Max allowable packet size */
#define MINBLOCK        64        /* Min allowable packet size */
#define MAXGOODNEEDED 8192        /* Max # good bytes req'd to bump packet size */
#define MAXRXTO        600
#define MINRXTO         10

/* Provision for future 7-bit ZMODEM; for now, there's no difference */
#define sendline xsendline

/*
 * Replacement for global variables normally used, in order to make code
 * fully reentrant; each invocation allocs their own Vars, and passes the
 * struct pointer down through the entire program so they're always available.
 * Pointer to this struct is usually able to be a register variable, so access
 * is no worse than any stack variable (all register-relative).  Kinda
 * kludgey, but the original ZModem code design depended on lots of globals,
 * and I didn't want to redesign the whole damn thing.  Besides, it's more
 * efficient than constantly pushing & popping args all over the place.
 */

struct Vars
{
  struct XPR_IO io;                   /* Copy of XProto IO struct passed by term prog. */
  struct XPR_UPDATE __aligned xpru;   /* Scratchpad xpr_update() control struct */
  struct timeval __aligned Starttime; /* Time transfer started */
  UBYTE __aligned Rxhdr[4];           /* Received header */
  UBYTE Txhdr[4];                     /* Transmitted header */
  UBYTE Msgbuf[128];                  /* Scratchpad buffer for printing messages */
  UBYTE Filename[PATHLEN];            /* Name of the file being up/downloaded */
  UBYTE Pktbuf[KSIZE];                /* File data packet buffer */
  UBYTE Modembuf[KSIZE * 2 + 256];    /* Input buffer for data from modem */
  UBYTE Outbuf[KSIZE * 2 + 256];      /* Output buffer for data to modem */  
  UBYTE *Modemchar;                   /* Next char to get from Modembuf */
  UBYTE *Filebuf;                     /* File I/O buffer address */
  UBYTE *Filebufptr;                  /* Current position within Filebuf */
  BPTR File;                          /* Handle of file being transferred */
  long Oldstatus;                     /* Original terminal program's modem settings */
  long Baud;                          /* BPS setting of modem */
  long Strtpos;                       /* Starting byte position of transfer */
  long Fsize;                         /* Size of file being transferred */
  long Rxbytes;                       /* Number of bytes received so far */
  long Filebufpos;                    /* File offset of data in Filebuf */
  long Filebufmax;                    /* Size of Filebuf */
  long Filebuflen;                    /* Number of bytes currently stored in Filebuf */
  long Filebufcnt;                    /* Number of bytes remaining/written in Filebuf */
  long Rxpos;                         /* Received file position */
  long Txpos;                         /* Transmitted file position */
  short Filcnt;                       /* Number of files opened for transmission */
  short Errcnt;                       /* Number of files unreadable */
  short Noroom;                       /* Flags 'insufficient disk space' errors */
  short Rxbuflen;                     /* Largest frame they're willing to xfer */
  short Tframlen;                     /* Largest frame we're willing to xfer */
  short Rxtimeout;                    /* Tenths of seconds to wait for something */
  short Tryzhdrtype;                  /* Header type to send corresp to Last rx close */
  short Modemcount;                   /* Number of bytes available in Modembuf */
  short Outbuflen;                    /* Number of bytes currently stored in Outbuf */
  short Rxframeind;                   /* ZBIN or ZHEX; type of frame received */
  short Txfcs32;                      /* TRUE means send binary frame with 32 bit FCS */
  short Rxflags;                      /* Temp register */
  short Wantfcs32;                    /* want to send 32 bit FCS */
  short Crc32t;                       /* Display flag indicates 32 bit CRC being sent */
  short Crc32;                        /* Display flag indicates 32 bit CRC being recd */
  short Rxtype;                       /* Type of header received */
  short Rxcount;                      /* Count of data bytes received */
  short Znulls;                       /* Number of nulls to send at begin of ZDATA hdr */
  short ErrorLimit;                   /* How many sequential errors before abort */
  char Rxbinary;                      /* Force binary mode download? */
  char Rxascii;                       /* Force text mode download? */
  char Thisbinary;                    /* Receive this file in binary mode? */
  char Lzconv;                        /* Suggested binary/text mode for uploads */
  char Eofseen;                       /* Text-mode EOF marker (^Z) rec'd on download? */
  UBYTE Zconv;                        /* ZMODEM file conversion request */
  UBYTE Zmanag;                       /* ZMODEM file management request */
  UBYTE Ztrans;                       /* ZMODEM file transport request */
  UBYTE Lastsent;                     /* Last text char written by putsec() */
  UBYTE Lastzsent;                    /* Last char sent by zsendline() */
  UBYTE Fileflush;                    /* Flush file I/O buffer before closing? */
  UBYTE Attn[ZATTNLEN + 1];           /* Attention string rx sends to tx on err */
  long Ksize;                         /* Max allowable packet size for this session */
  long NewBaud;                       /* BPS link rate passed in xpr options */
  char FTNmode;                       /* change parameters for FTN operations */
  char NoFiles;                       /* allow no file send for FTN operations */
  char SmallBlocks;                   /* we are in pure zmodem 1024 bytes  */ 
  char dzap;                          /* escape only ZDLE/ZDLEE if true */
  char NoMask;                        /* disable XPR 2.001/3 masks */
  short StartTimeout; 
};

/*
 * Option settings and other variables needed outside of XProtocolSend/Receive;
 * separated from rest of Vars so that huge Vars struct doesn't have to be
 * allocated except during transfers.  Pointer to this struct kept in xpr_data.
 */
struct SetupVars
  {
    UBYTE *matchptr, *bufpos;
    short buflen;
    UBYTE option_t[2], option_o[2], option_b[8], option_f[8], option_e[8], option_s[4];
    UBYTE option_r[4], option_a[4], option_d[4], option_k[4], option_p[256], option_m[8];
    UBYTE option_c[8], option_n[4], option_q[4], option_z[4], option_y[4], option_x[8];
  };

/* Function prototypes */

long __saveds __asm XProtocolSend (register __a0 struct XPR_IO *xio);
short getzrxinit (struct Vars *v);
void sendbatch (struct Vars *v);
short sendone (struct Vars *v);
short sendname (struct Vars *v);
short zsendfile (struct Vars *v, short blen);
short zsendfdata (struct Vars *v);
short getinsync (struct Vars *v);
void saybibi (struct Vars *v);

long __saveds __asm XProtocolReceive (register __a0 struct XPR_IO *xio);
short rcvbatch (struct Vars *v);
short tryz (struct Vars *v);
short rzfiles (struct Vars *v);
short rzfile (struct Vars *v);
short procheader (struct Vars *v);
short putsec (struct Vars *v);
void ackbibi (struct Vars *v);

long __saveds __asm XProtocolSetup (register __a0 struct XPR_IO *xio);
long __saveds __asm XProtocolCleanup (register __a0 struct XPR_IO *xio);
long __saveds __asm XProtocolHostMon (
                                       register __a0 struct XPR_IO *xio,
                                       register __a1 char *serbuff,
                                       register __d0 long actual,
                                       register __d1 long maxsize);
long __saveds __asm XProtocolUserMon (
                                       register __a0 struct XPR_IO *xio,
                                       register __a1 char *serbuff,
                                       register __d0 long actual,
                                       register __d1 long maxsize);
struct Vars *setup (struct XPR_IO *io);
UBYTE *find_option (UBYTE * buf, UBYTE option);
void set_textmode (struct Vars *v);
void canit (struct Vars *v);
void zmputs (struct Vars *v, UBYTE * s);
void xsendline (struct Vars *v, UBYTE c);
void sendbuf (struct Vars *v);
short readock (struct Vars *v, short tenths);
char char_avail (struct Vars *v);
void update_rate (struct Vars *v);
BPTR bfopen (struct Vars *v, UBYTE * mode); 
/*void *bfopen (struct Vars *v, UBYTE * mode);  ZEDZAP */
void bfclose (struct Vars *v);
void bfseek (struct Vars *v, long pos);
long bfread (struct Vars *v, UBYTE * buf, long length);
long bfwrite (struct Vars *v, UBYTE * buf, long length);
void ioerr (struct XPR_IO *io, char *msg);
void updstatus (struct Vars *v, char *filename, long status, long mask);
void upderr (struct Vars *v, char *msg, long mask);
void updmsg (struct Vars *v, char *msg, long mask);
long getfree (void);
char exist (struct Vars *v);

#ifdef DEBUGLOG
void dlog (struct Vars *v, UBYTE * s);
#define DEBUGINFO (xprsprintf(v->Msgbuf, "Source: %-10s  Func: %-12s  Line: %4ld\n",__FILE__,__FUNC__,__LINE__), dlog(v, v->Msgbuf))

#ifdef DEBUGLEVEL2
#define D(x) x
#else
#define D(x) ;
#endif
#endif

void zsbhdr (struct Vars *v, USHORT type);
void zshhdr (struct Vars *v, USHORT type);
void zsdata (struct Vars *v, short length, USHORT frameend);
short zrdata (struct Vars *v, UBYTE * buf, short length);
short zrdat32 (struct Vars *v, UBYTE * buf, short length);
short zgethdr (struct Vars *v);
short zrbhdr (struct Vars *v);
short zrbhdr32 (struct Vars *v);
short zrhhdr (struct Vars *v);
void zputhex (struct Vars *v, UBYTE c);
void zsendline (struct Vars *v, UBYTE c);
short zgethex (struct Vars *v);
short zdlread (struct Vars *v);
short noxrd7 (struct Vars *v);
void stohdr (struct Vars *v, long pos);
long rclhdr (struct Vars *v);

/* xprzmodem_timers.c */
extern ULONG UnixTimeOffset;
ULONG getsystime (struct timeval *tv);
void XprTimeOut(long ticks);
LONG xprsprintf (STRPTR buffer, STRPTR fmt,...);

struct LocaleInfo;

extern STRPTR GetLocalString(struct LocaleInfo *, LONG);

#ifdef KDEBUG
void KPrintF(UBYTE *fmt,...);
#endif

/* callbacks */
/*  callbacks.a  is  glue to call XPR callback functions in comm program from */
/* inside XPR library.  This protects our registers from potential bugs in the */
/* comm program which might change them in unexpected ways.  */
/*  The  prototypes  put all arguments into the registers required by the XPR */
/* spec, so we don't have to move anything around. */

void * __asm xpr_fopen(register __a3 struct XPR_IO *io,register __a0 char *filename,register __a1 char *accessmode);
void __asm xpr_fclose(register __a3 struct XPR_IO *io,register __a0 void *fileptr);
long __asm xpr_fread(register __a3 struct XPR_IO *io,register __a0 char *buffer,register __d0 long size,register __d1 long count,register __a1 void *fileptr);
long __asm xpr_fwrite(register __a3 struct XPR_IO *io,register __a0 char *buffer,register __d0 long size,register __d1 long count,register __a1 void *fileptr);
long __asm xpr_sread(register __a3 struct XPR_IO *io,register __a0 char *buffer,register __d0 long size,register __d1 long timeout);
long __asm xpr_swrite(register __a3 struct XPR_IO *io,register __a0 char *buffer,register __d0 long size);
long __asm xpr_sflush(register __a3 struct XPR_IO *io);
void __asm xpr_update(register __a3 struct XPR_IO *io,register __a0 struct XPR_UPDATE *updatestruct);
long __asm xpr_chkabort(register __a3 struct XPR_IO *io);
long __asm xpr_chkmisc(register __a3 struct XPR_IO *io);
long __asm xpr_gets(register __a3 struct XPR_IO *io,register __a0 char *prompt,register __a1 char *buffer);
long __asm xpr_setserial(register __a3 struct XPR_IO *io,register __d0 long newstatus);
long __asm xpr_ffirst(register __a3 struct XPR_IO *io,register __a0 char *buffer,register __a1 char *pattern);
long __asm xpr_fnext(register __a3 struct XPR_IO *io,register __d0 long oldstate,register __a0 char *buffer,register __a1 char *pattern);
long __asm xpr_finfo(register __a3 struct XPR_IO *io,register __a0 char *filename,register __d0 long typeofinfo);
long __asm xpr_fseek(register __a3 struct XPR_IO *io,register __a0 void *fileptr,register __d0 long offset,register __d1 long origin);
long __asm xpr_options(register __a3 struct XPR_IO *io,register __d0 long n,register __a0 struct xpr_option **opt);
long __asm xpr_unlink(register __a3 struct XPR_IO *io,register __a0 char *filename);
long __asm xpr_squery(register __a3 struct XPR_IO *io);
long * __asm xpr_getptr(register __a3 struct XPR_IO *io,register __d0 long type);
/* End of XprZmodem.h source */
