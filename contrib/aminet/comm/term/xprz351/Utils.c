 /**********************************************************************
  * Utils.c: Miscellaneous support routines for xprzmodem.library;
  * Version 2.10, 12 February 1991, by Rick Huebner.
  * Released to the Public Domain; do as you like with this code.
  *
  * Version 2.50, 15 November 1991, by William M. Perkins.  Added code
  * to update_rate() function in utils.c to avoid the Guru # 80000005
  * you would have gotten if you had decided to adjust the system clock
  * back during an upload or download.
  *
  * Mysprintf() function to replace sprintf() and proto code to use
  * libinit.o and linent.o library code was supplied by Jim Cooper of SAS.
  *
  * Version 2.61, 3 July 1993 Mysprintf changed to xprsprintf(), written
  * in Assembler, because under SAS/C the old code running not correctly.
  *
  * Version 2.62, 27 July 1993 build in variable Blocksize.
  *
  * Version 2.63, 30 July 1993 build in locale, by Rainer Hess
  *
  * Version 2.64,  3 Aug 1993 global vaiable Blocksize, now in
  *                 struct Vars, by Rainer Hess
  *
  *                4 Aug 1993 changes in update_rate() function because
  * it always GURU 8000 0005 on little files (testet with 2 byte-file)
  * when you send. If on receiver the file exists when receiver ask for
  * overwrite, the machine crash if you click overwrite on receiver.
  * Testet on Term 3.4 -> Ncomm 2.0, by Rainer Hess
  *
  * Version 3.2   11 Nov 1994 modifications to implement FTN operations
  *               by Roert Williamson
  * Version 3.2   19 Nov 1994 version string added, XPR 2.001 support added
  * Version 3.5   Jan 1,1994 added rxtimeout, xpr2ext setup options   
  * Version 3.52  June 1995 , fix incorrect values for certain setup options
  **********************************************************************/


#define CATCOMP_NUMBERS
#include "xprzmodem_catalog.h"

#include "xprzmodem_all.h"

/*
 * Transfer options to use if XProtocolSetup not called
 */
#ifdef zedzap
struct SetupVars Default_Config =
{
  NULL,       /* = *matchptr  */
  NULL,       /* = *bufpos    */
  0,          /* = buflen     */
  "N",        /* option_t[2]  Set Text translation mode */
  "R",        /* option_o[2]  Overwrite mode */
  "16",       /* option_b[8]  Buffer size */
  "0",        /* option_f[8]  Frame size */
  "30",       /* option_e[8]  Error count */
  "N",        /* option_s[4]  Send full directory path */
  "N",        /* option_r[4]  Receive full directory path */
  "N",        /* option_a[4]  Auto-activate mode */
  "N",        /* option_d[4]  Delete after sending */
  "Y",        /* option_k[4]  Keep partial files */
  "",         /* option_p[256] Path to use for received files */
  "8192",     /* option_m[8]  Set maximum packet size */
  "0",        /* option_c[8]  Set Link BPS Rate */
  "Y",        /* option_n[4]  Send if there are no files */
  "N",        /* option_q[4]  Enable DirectZap escape only CAN */
  "Y",        /* option_z[4]  Enable FTN mode */
  "Y",        /* option_y[4]  Enable XPR 2.001 results */
  "100"       /* option_x[8]  StartTimeout */
};
#else
struct SetupVars Default_Config =
{
  NULL,       /* = *matchptr  */
  NULL,       /* = *bufpos    */
  0,          /* = buflen     */
  "C",        /* option_t[2]  Set Text translation mode */
  "N",        /* option_o[2]  Overwrite mode */
  "16",       /* option_b[8]  Buffer size */
  "0",        /* option_f[8]  Frame size */
  "10",       /* option_e[8]  Error count */
  "N",        /* option_s[4]  Send full directory path */
  "N",        /* option_r[4]  Receive full directory path */
  "Y",        /* option_a[4]  Auto-activate mode */
  "N",        /* option_d[4]  Delete after sending */
  "Y",        /* option_k[4]  Keep partial files */
  "",         /* option_p[256] Path to use for received files */
  "1024",     /* option_m[8]  Set maximum packet size */
  "0",        /* option_c[8]  Set Link BPS Rate */
  "N",        /* option_n[4]  Send if there are no files */
  "N",        /* option_q[4]  Enable DirectZap escape only CAN */
  "N",        /* option_z[4]  Enable FTN mode */
  "N",        /* option_y[4]  Enable XPR 2.001 results */
  "100"       /* option_x[8]  StartTimeout */
};
#endif

#ifdef DEBUGLOG
UBYTE DebugName[] = "T:XDebug.Log";
void *DebugLog = NULL;
#endif

#ifndef __AROS__
struct DosLibrary *DOSBase;
struct ExecBase *SysBase;
#endif

#ifdef UP_TO_2X
struct Library *UtilityBase;
#endif

#define LIBRARY_ANY 0L

#define LocaleBase li.li_LocaleBase
#define catalog    li.li_Catalog

struct LocaleInfo li;

struct TagItem LocalTags[3] =
{
  OC_BuiltInLanguage, (ULONG) "english",
  OC_Version, 1,
  TAG_DONE
};

/**********************************************************
 *      int __UserLibInit(struct Library *libbase)
 **********************************************************/
int __saveds __asm
__UserLibInit (register __a6 struct MyLibrary *libbase)
{
#ifndef __AROS__
  SysBase = *(struct ExecBase **) 4L;
#endif
  if (!(DOSBase = (struct DosLibrary *) OpenLibrary ("dos.library", LIBRARY_ANY)))
    return (RETURN_FAIL);

#ifdef UP_TO_2X
  if (!(UtilityBase = OpenLibrary ("utility.library", 37L)))
    return (RETURN_FAIL);
#endif

  if (LocaleBase = OpenLibrary ("locale.library", 38L))
    catalog = OpenCatalogA (NULL, "xprzmodem.catalog", &LocalTags[0]);

  return (RETURN_OK);
}                                /* End of __UserLibInit() */

/**********************************************************
 *      void __UserLibCleanup(struct Library *libbase)
 **********************************************************/
void __saveds __asm
__UserLibCleanup (register __a6 struct MyLibrary *libbase)
{
  if (DOSBase)
    CloseLibrary ((struct Library *) DOSBase);

#ifdef UP_TO_2X
  if (UtilityBase)
    CloseLibrary (UtilityBase);
#endif

  if (LocaleBase)
    {
      if (catalog)
        CloseCatalog (catalog);
      CloseLibrary (LocaleBase);
    }
}                                /* End of __UserLibCleanup() */

/**********************************************************
 *      long XProtocolSetup(struct XPR_IO *xio)
 *
 * Called by comm program to set transfer options
 **********************************************************/
long __saveds __asm
XProtocolSetup (register __a0 struct XPR_IO *xio)
{
  struct SetupVars *sv, tempsv;
  struct xpr_option *option_ptrs[19];
  struct xpr_option *optr, xo_hdr, 
    xo_t, xo_o, xo_b, xo_f, xo_e, xo_s,
    xo_r, xo_a, xo_d, xo_k, xo_p, xo_m, 
    xo_c, xo_n, xo_q, xo_z, xo_y, xo_x;
  UBYTE buf[512], *p;
  long i, len;

  /* Allocate memory for transfer options string */
  if (!(sv = (void *) xio->xpr_data))
  {
/*      xio->xpr_data = AllocMem ((ULONG) sizeof (struct SetupVars), MEMF_CLEAR); */
      xio->xpr_data = AllocMem ((long) sizeof (struct SetupVars), MEMF_CLEAR);
      if (!(sv = (void *) xio->xpr_data))
        {
          ioerr (xio, "Not enough memory!");
          return XPRS_FAILURE;
        }
      /* Start out with default options; merge user changes into defaults */
      *sv = Default_Config;
  }

  /* If options string passed by comm prog, use it; else prompt user */
  if (xio->xpr_filename)
    strcpy (buf, xio->xpr_filename);
  else
  {
    /* If xpr_options() implemented by comm program, use it */
    if (xio->xpr_extension >= 1 && xio->xpr_options)
    {
        /*
           * Let user edit temp copy of options so we can ignore invalid
           * entries.  Have to init all this crud the hard way 'cause it's
           * got to be on the stack in order to maintain reentrancy
         */
        tempsv = *sv;
        xo_hdr.xpro_description = GetLocalString( &li, MSG_ZMODEM_OPTIONS );
        xo_hdr.xpro_type = XPRO_HEADER;
        xo_hdr.xpro_value = NULL;
        xo_hdr.xpro_length = 0;
        option_ptrs[0] = &xo_hdr;

        xo_t.xpro_description = GetLocalString( &li, MSG_TEXT_MODE );
        xo_t.xpro_type = XPRO_STRING;
        xo_t.xpro_value = tempsv.option_t;
        xo_t.xpro_length = sizeof (tempsv.option_t);
        option_ptrs[1] = &xo_t;

        xo_o.xpro_description = GetLocalString( &li, MSG_OVERWRITE_MODE );
        xo_o.xpro_type = XPRO_STRING;
        xo_o.xpro_value = tempsv.option_o;
        xo_o.xpro_length = sizeof (tempsv.option_o);
        option_ptrs[2] = &xo_o;

        xo_b.xpro_description = GetLocalString( &li, MSG_IO_BUFFER_SIZE );
        xo_b.xpro_type = XPRO_LONG;
        xo_b.xpro_value = tempsv.option_b;
        xo_b.xpro_length = sizeof (tempsv.option_b);
        option_ptrs[3] = &xo_b;

        xo_f.xpro_description = GetLocalString( &li, MSG_FRAME_SIZE );
        xo_f.xpro_type = XPRO_LONG;
        xo_f.xpro_value = tempsv.option_f;
        xo_f.xpro_length = sizeof (tempsv.option_f);
        option_ptrs[4] = &xo_f;

        xo_e.xpro_description = GetLocalString( &li, MSG_ERROR_LIMIT );
        xo_e.xpro_type = XPRO_LONG;
        xo_e.xpro_value = tempsv.option_e;
        xo_e.xpro_length = sizeof (tempsv.option_e);
        option_ptrs[5] = &xo_e;

        xo_a.xpro_description = GetLocalString( &li, MSG_AUTO_ACTIVATE_RECEIVER );
        xo_a.xpro_type = XPRO_BOOLEAN;
        xo_a.xpro_value = tempsv.option_a;
        xo_a.xpro_length = sizeof (tempsv.option_a);
        option_ptrs[6] = &xo_a;

        xo_d.xpro_description = GetLocalString( &li, MSG_DELETE_AFTER_SENDING );
        xo_d.xpro_type = XPRO_BOOLEAN;
        xo_d.xpro_value = tempsv.option_d;
        xo_d.xpro_length = sizeof (tempsv.option_d);
        option_ptrs[7] = &xo_d;

        xo_k.xpro_description = GetLocalString( &li, MSG_KEEP_PARTIAL_FILES );
        xo_k.xpro_type = XPRO_BOOLEAN;
        xo_k.xpro_value = tempsv.option_k;
        xo_k.xpro_length = sizeof (tempsv.option_k);
        option_ptrs[8] = &xo_k;

        xo_s.xpro_description = GetLocalString( &li, MSG_SEND_FULL_PATH );
        xo_s.xpro_type = XPRO_BOOLEAN;
        xo_s.xpro_value = tempsv.option_s;
        xo_s.xpro_length = sizeof (tempsv.option_s);
        option_ptrs[9] = &xo_s;

        xo_r.xpro_description = GetLocalString( &li, MSG_USE_RECEIVED_PATH );
        xo_r.xpro_type = XPRO_BOOLEAN;
        xo_r.xpro_value = tempsv.option_r;
        xo_r.xpro_length = sizeof (tempsv.option_r);
        option_ptrs[10] = &xo_r;

        xo_p.xpro_description = GetLocalString( &li, MSG_DEFAULT_RECEIVE_PATH );
        xo_p.xpro_type = XPRO_STRING;
        xo_p.xpro_value = tempsv.option_p;
        xo_p.xpro_length = sizeof (tempsv.option_p);
        option_ptrs[11] = &xo_p;

        xo_m.xpro_description = GetLocalString( &li, MSG_MAXIMUM_BLOCK_SIZE );
        xo_m.xpro_type = XPRO_LONG;
        xo_m.xpro_value = tempsv.option_m;
        xo_m.xpro_length = sizeof (tempsv.option_m);
        option_ptrs[12] = &xo_m;

        xo_c.xpro_description = GetLocalString( &li, MSG_LINK_RATE );
        xo_c.xpro_type = XPRO_LONG;
        xo_c.xpro_value = tempsv.option_c;
        xo_c.xpro_length = sizeof (tempsv.option_c);
        option_ptrs[13] = &xo_c;

        xo_n.xpro_description = GetLocalString( &li, MSG_SEND_NO_FILES );
        xo_n.xpro_type = XPRO_BOOLEAN;
        xo_n.xpro_value = tempsv.option_n;
        xo_n.xpro_length = sizeof (tempsv.option_n);
        option_ptrs[14] = &xo_n;

        xo_q.xpro_description = GetLocalString( &li, MSG_ESCAPE_ONLY_ZCAN );
        xo_q.xpro_type = XPRO_BOOLEAN;
        xo_q.xpro_value = tempsv.option_q;
        xo_q.xpro_length = sizeof (tempsv.option_q);
        option_ptrs[15] = &xo_q;

        xo_z.xpro_description = GetLocalString( &li, MSG_FTN_ZMODEM );
        xo_z.xpro_type = XPRO_BOOLEAN;
        xo_z.xpro_value = tempsv.option_z;
        xo_z.xpro_length = sizeof (tempsv.option_z);
        option_ptrs[16] = &xo_z;

        xo_y.xpro_description = GetLocalString( &li, MSG_XPR_EXT );
        xo_y.xpro_type = XPRO_BOOLEAN;
        xo_y.xpro_value = tempsv.option_y;
        xo_y.xpro_length = sizeof (tempsv.option_y);
        option_ptrs[17] = &xo_y;

        xo_x.xpro_description = GetLocalString( &li, MSG_STARTUP_TIMEOUT );
        xo_x.xpro_type = XPRO_LONG;
        xo_x.xpro_value = tempsv.option_x;
        xo_x.xpro_length = sizeof (tempsv.option_x);
        option_ptrs[18] = &xo_x;


        /* Convert Y/N used elsewhere into "yes"/"no" required by spec */
        for (i = 6; i <= 10; ++i)
        {
          optr = option_ptrs[i];
          strcpy (optr->xpro_value, (*optr->xpro_value == 'Y') ? "yes" : "no");
        }
        for (i = 14; i <= 17; ++i)
        {
          optr = option_ptrs[i];
          strcpy (optr->xpro_value, (*optr->xpro_value == 'Y') ? "yes" : "no");
        }

        (*xio->xpr_options) (19L, option_ptrs);

        /* Convert "yes"/"no" or "on"/"off" into Y/N */
        for (i = 6; i <= 10; ++i)
        {
          optr = option_ptrs[i];
          strcpy (optr->xpro_value, (!stricmp (optr->xpro_value, "yes")
                     || !stricmp (optr->xpro_value, "on")) ? "Y" : "N");
        }
        for (i = 14; i <= 17; ++i)
        {
          optr = option_ptrs[i];
          strcpy (optr->xpro_value, (!stricmp (optr->xpro_value, "yes")
                     || !stricmp (optr->xpro_value, "on")) ? "Y" : "N");
        }
        /* Convert xpr_options() results into parseable options string */
        xprsprintf (buf, "T%s,O%s,B%s,F%s,E%s,A%s,D%s,K%s,S%s,R%s,P%s,M%s,C%s,N%s,Q%s,Z%s",
          tempsv.option_t, tempsv.option_o, tempsv.option_b, tempsv.option_f,
          tempsv.option_e, tempsv.option_a, tempsv.option_d, tempsv.option_k,
          tempsv.option_s, tempsv.option_r, tempsv.option_p, tempsv.option_m,
          tempsv.option_c, tempsv.option_n, tempsv.option_q, tempsv.option_z,
          tempsv.option_y, tempsv.option_x);
        /* If xpr_options() not provided, try xpr_gets() instead */
    }
    else
    {
      /* Start buffer with current settings so user can see/edit them. */
      xprsprintf (buf, "T%s,O%s,B%s,F%s,E%s,A%s,D%s,K%s,S%s,R%s,P%s,M%s,C%s,N%s,Q%s,Z%s",
          sv->option_t, sv->option_o, sv->option_b, sv->option_f,
          sv->option_e, sv->option_a, sv->option_d, sv->option_k,
          sv->option_s, sv->option_r, sv->option_p, sv->option_m,
          sv->option_c, sv->option_n, sv->option_q, sv->option_z,
          sv->option_y, sv->option_x);
      if (xio->xpr_gets)
        (*xio->xpr_gets) ( GetLocalString( &li, MSG_ZMODEM_OPTIONS ), buf);
    }
  }
  /* Upshift options string for easier parsing */
  strupr (buf);

  /*
     * Merge new T(ext) option into current settings if given
     *  "TY" = Force Text mode on,
     *  "TN" = Force Text mode off,
     *  "T?" = Use other end's text mode suggestion (default to binary)
     *  "TC" = Ask Comm program for file type
   */
  if (p = find_option (buf, 'T'))
  {
    if (*p == 'Y' || *p == 'N' || *p == '?' || *p == 'C') *sv->option_t = *p;
      else ioerr (xio,  GetLocalString( &li, MSG_INVALID_T_FLAG ));
  }

  /*
     * Merge new O(verwrite) option into current settings if given
     *  "OY" = Yes, delete old file and replace with new one,
     *  "ON" = No, prevent overwrite by appending ".dup" to avoid name collision,
     *  "OR" = Resume transfer at end of existing file,
     *  "OS" = Skip file if it already exists; go on to next
   */
  if (p = find_option (buf, 'O'))
  {
    if (*p == 'R' && !xio->xpr_finfo)
      ioerr (xio, GetLocalString( &li, MSG_NO_XPR_FINFO ));
    else if (*p == 'Y' || *p == 'N' || *p == 'R' || *p == 'S')
      *sv->option_o = *p;
    else ioerr (xio, GetLocalString( &li, MSG_INVALID_O_FLAG ));
  }

  /*
     * Merge new B(uffer) setting into current settings if given
     * Size of file I/O buffer in kilobytes
   */
  if (p = find_option (buf, 'B'))
  {
    len = atol (p);
    if (len < 1) len = 1;
    xprsprintf (sv->option_b, "%ld", len);
  }

  /*
     * Merge new F(ramelength) setting into other settings if given
     * Number of bytes we're willing to send or receive between ACKs.
     * 0 = unlimited; nonstop streaming data
   */
  if (p = find_option (buf, 'F'))
  {
    len = atol (p);
    if (len < 0) len = 0;
    if (len > 0 && len < MINBLOCK) len = MINBLOCK;
    xprsprintf (sv->option_f, "%ld", len);
  }

  /*
     * Merge new E(rror limit) setting into other settings if given
     * Number of sequential errors which will cause an abort
   */
  if (p = find_option (buf, 'E'))
  {
    len = atol (p);
    if (len < 1) len = 1;
    if (len > 32767) len = 32767;
    xprsprintf (sv->option_e, "%ld", len);
  }

  /*
     * Merge new A(uto-activate) setting into other settings if given
     *  "AY" = Automatically call XProtocolReceive() if ZRQINIT string received
     *  "AN" = Don't look for ZRQINIT; user will explicitly activate receive
   */
  if (p = find_option (buf, 'A'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_a = *p;
      else ioerr (xio, GetLocalString( &li, MSG_INVALID_A_FLAG ));
  }

  /*
     * Merge new D(elete after sending) setting into other options
     *  "DY" = Delete files after successfully sending them
     *  "DN" = Don't delete files after sending
   */
  if (p = find_option (buf, 'D'))
  {
    if (*p == 'Y' && (xio->xpr_extension < 2 || !xio->xpr_unlink))
      ioerr (xio, GetLocalString( &li, MSG_NO_DY_XPR_UNLINK ));
    else if (*p == 'Y' || *p == 'N') *sv->option_d = *p;
    else ioerr (xio, GetLocalString( &li, MSG_INVALID_D_FLAG ));
  }

  /*
     * Merge new K(eep partial files) setting into other options
     *  "KY" = Keep partially-received file fragments to allow later resumption
     *  "KN" = Delete partially-received file fragments
   */
  if (p = find_option (buf, 'K'))
  {
    if (*p == 'N' && (xio->xpr_extension < 2 || !xio->xpr_unlink))
      ioerr (xio, GetLocalString( &li, MSG_NO_KN_XPR_UNLINK ));
    else if (*p == 'Y' || *p == 'N') *sv->option_k = *p;
    else ioerr (xio, GetLocalString( &li, MSG_INVALID_K_FLAG ));
  }

  /*
     * Merge new S(end full path) setting into other options
     *  "SY" = Send full filename including directory path to receiver
     *  "SN" = Send only simple filename portion, not including directory path
   */
  if (p = find_option (buf, 'S'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_s = *p;
      else ioerr (xio, GetLocalString( &li, MSG_INVALID_S_FLAG ));
  }

  /*
     * Merge new R(eceive path) setting into other options
     *  "RY" = Use full filename exactly as received; don't use P option path
     *  "RN" = Ignore received directory path if any; use path from P option
   */
  if (p = find_option (buf, 'R'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_r = *p;
      else ioerr (xio, GetLocalString( &li, MSG_INVALID_R_FLAG ));
  }

  /*
     * Merge new P(ath) setting into other options
     *  "Pdir" = Receive files into directory "dir" if RN selected
     *  "dir" can by any valid existing directory, with or without trailing "/"
   */
  if (p = find_option (buf, 'P'))
  {
    strcpy (sv->option_p, p);
    p = sv->option_p + strcspn (sv->option_p, " ,\t\r\n");
    *p = '\0';
  }

  /* Maximum packet size. Must be <=8192 */

  if (p = find_option (buf, 'M'))
  {
    len = atol (p);
    if (len < MINBLOCK) len = MINBLOCK;
    if (len > MAXGOODNEEDED || len > KSIZE) len = KSIZE;
    xprsprintf (sv->option_m, "%ld", len);
  }

  /* Modify BPS Rate If We Have The BPS Rate Locked */
  if (p = find_option(buf,'C'))
  {
    len = atol(p);
    if ((len < 300)||(len > 58600)) len = 0;
    xprsprintf(sv->option_c,"%ld",len);
  }

  if (p = find_option(buf,'N'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_n = *p;
      else ioerr(xio,GetLocalString( &li, MSG_INVALID_N_FLAG ));
  }
  if (p = find_option(buf,'Q'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_q = *p;
      else ioerr(xio,GetLocalString( &li, MSG_INVALID_Q_FLAG ));
  }
  if (p = find_option(buf,'Z'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_z = *p;
      else ioerr(xio, GetLocalString( &li, MSG_INVALID_Z_FLAG ));
  }
  if (p = find_option (buf, 'Y'))
  {
    if (*p == 'Y' || *p == 'N') *sv->option_y = *p;
      else ioerr (xio, GetLocalString( &li, MSG_INVALID_Y_FLAG ));
  }
  if (p = find_option (buf, 'X'))
  {
    len = atol (p);
    if (len < MINRXTO) len = MINRXTO;
    if (len > MAXRXTO) len = MAXRXTO;
    xprsprintf (sv->option_x, "%ld", len);
  }


#ifdef KDEBUG
KPrintF("XProtocolSetup\nT%s,O%s,B%s,F%s,E%s,A%s\nD%s,K%s,S%s,R%s,P%s,M%s\nC%s,N%s,Q%s,Z%s\n",
                  sv->option_t, sv->option_o, sv->option_b, sv->option_f,
                  sv->option_e, sv->option_a, sv->option_d, sv->option_k,
                  sv->option_s, sv->option_r, sv->option_p, sv->option_m,
                  sv->option_c, sv->option_n, sv->option_q, sv->option_z,
                  sv->option_y, sv->option_x);

#endif
  if (*sv->option_y == 'N')
  {
    return (*sv->option_a == 'Y') ? 
      XPRS_SUCCESS | XPRS_NORECREQ | XPRS_HOSTMON :
      XPRS_SUCCESS | XPRS_NORECREQ;
  }
  else
  {
    return (*sv->option_a == 'Y') ? 
      XPRS_SUCCESS | XPRS_NORECREQ | XPRS_HOSTMON | XPRS_XPR2001 | XPRS_DOUBLE :
      XPRS_SUCCESS | XPRS_NORECREQ | XPRS_XPR2001 | XPRS_DOUBLE;
  }
}                                /* End of long XProtocolSetup() */

/**********************************************************
 *      long XProtocolCleanup(struct XPR_IO *xio)
 *
 * Called by comm program to give us a chance to clean
 * up before program ends
 **********************************************************/
long __saveds __asm
XProtocolCleanup (register __a0 struct XPR_IO *xio)
{
  /* Release option memory, if any */
  if (xio->xpr_data)
  {
    FreeMem (xio->xpr_data, (long) sizeof (struct SetupVars));
    xio->xpr_data = NULL;
  }

  return XPRS_SUCCESS;
}                                /* End of long XProtocolCleanup() */

/**********************************************************
 *      long XProtocolHostMon(struct XPR_IO *xio,
 *         char *serbuff, long actual, long maxsize)
 *
 * Called by comm program upon our request (XPRS_HOSTMON)
 * to let us monitor the incoming data stream for our
 * receiver auto-activation string (ZRQINIT packet).
 * We only ask for this to be called if option AY is set.
 **********************************************************/
long __saveds __asm
XProtocolHostMon (
                   register __a0 struct XPR_IO *xio,
                   register __a1 char *serbuff,
                   register __d0 long actual,
                   register __d1 long maxsize)
{
  static UBYTE startrcv[] =
  {
    ZPAD, ZDLE, ZHEX, '0', '0'
  };
  struct SetupVars *sv;

  if (!(sv = (void *) xio->xpr_data))
    return actual;                /* XProtocolSetup() never called?! */

  if (!sv->matchptr)
    sv->matchptr = startrcv;

  /*
     * Scan through serbuff to see if we can match all bytes in the start
     * string in sequence.
  */
  for (sv->bufpos = serbuff; sv->bufpos < serbuff + actual; ++sv->bufpos)
  {
    if (*sv->bufpos == *sv->matchptr)
    {                        /* if data matches current position in match */
      ++sv->matchptr;        /* string, increment match position */
      if (!*sv->matchptr)
      {                        /* if at end of match string, it all matched */
        sv->buflen = actual - ((long)sv->bufpos - (long)serbuff);
        XProtocolReceive (xio);
        sv->matchptr = startrcv;
        actual = 0;
        break;
      }
    }
    else if (sv->matchptr > startrcv)
    {                        /* mismatch?  Reset to start of match string */
      sv->matchptr = startrcv;
      if (*sv->bufpos == *sv->matchptr)
        ++sv->matchptr;
    }
  }
  sv->bufpos = NULL;
  return actual;
}

/**********************************************************
 *      long XProtocolUserMon(struct XPR_IO *xio,
 *         char *serbuff, long actual, long maxsize)
 *
 * Called by comm program to let us monitor user's inputs;
 * we never ask for this to be called, but it's better to
 * recover gracefully than guru the machine.
 **********************************************************/
long __saveds __asm
XProtocolUserMon (
                   register __a0 struct XPR_IO *xio,
                   register __a1 char *serbuff,
                   register __d0 long actual,
                   register __d1 long maxsize)
{
  return actual;
}

/**********************************************************
 *      struct Vars *setup(struct XPR_IO *io)
 *
 * Perform setup and initializations common to both Send
 * and Receive routines
 **********************************************************/
struct Vars *
setup (struct XPR_IO *io)
{
/*
  static long bauds[] =
  {
    110, 300, 1200, 2400,
    4800, 9600, 19200, 31250,
    38400, 57600, 76800, 115200
  };
*/
  static long bauds[] = 
  { 
    300,600,1200,2400,
    4800,7200,9600,12000,
    14400,16800,19200,31250,
    38400,57600,76800,115200
  };

  struct SetupVars *sv;
  struct Vars *v;
  long origbuf, newstatus;

#ifdef DEBUGLOG
  long i, *lng;
#endif

  /* Make sure comm program supports the required call-back functions */
  if (!io->xpr_update)
    return NULL;

  if (!io->xpr_fopen || !io->xpr_fclose || !io->xpr_fread
      || !io->xpr_fwrite || !io->xpr_fseek || !io->xpr_sread
      || !io->xpr_swrite)
  {
    ioerr (io, GetLocalString( &li, MSG_COMM_PROG_MISSING ));
    return NULL;
  }

  /* Hook in default transfer options if XProtocolSetup wasn't called */
  if (!(sv = (void *) io->xpr_data))
  {
/*    io->xpr_data = AllocMem ((ULONG) sizeof (struct SetupVars), MEMF_CLEAR);  */
    io->xpr_data = AllocMem ((long) sizeof (struct SetupVars), MEMF_CLEAR);
    if (!(sv = (void *) io->xpr_data))
    {
      ioerr (io, GetLocalString( &li, MSG_NOT_ENOUGH_MEMORY ));
      return NULL;
    }
    *sv = Default_Config;
  }

  /* Allocate memory for our unshared variables, to provide reentrancy */
/*  if (!(v = AllocMem ((ULONG) sizeof (struct Vars), MEMF_CLEAR))) */
  if (!(v = AllocMem ((long) sizeof (struct Vars), MEMF_CLEAR)))
  {
  nomem:
    ioerr (io, GetLocalString( &li, MSG_NOT_ENOUGH_MEMORY ));
    return NULL;
  }
  v->Modemchar = v->Modembuf;

  /*
     * Allocate memory for our file I/O buffer; if we can't get as much as
     * requested, keep asking for less until we hit minimum before giving up
   */
  v->Filebufmax = origbuf = atol (sv->option_b) * 1024;
/*  while (!(v->Filebuf = AllocMem ((ULONG) v->Filebufmax, MEMF_ANY)))  */
  while (!(v->Filebuf = AllocMem (v->Filebufmax,0L)))
  {
    if (v->Filebufmax > 1024)  v->Filebufmax -= 1024;
    else
    {
      FreeMem (v, (long) sizeof (struct Vars));
      goto nomem;
    }
  }

 v->Ksize = atol(sv->option_m);
 v->StartTimeout=atol(sv->option_x);

 if((v->Ksize * 2) > v->Filebufmax)
  {
    if(v->Filebufmax >=  1024) { v->Ksize = 512;  strcpy(sv->option_m,"512" ); }
    if(v->Filebufmax >=  2048) { v->Ksize = 1024; strcpy(sv->option_m,"1024"); }
    if(v->Filebufmax >=  4096) { v->Ksize = 2048; strcpy(sv->option_m,"2048"); }
    if(v->Filebufmax >=  8192) { v->Ksize = 4096; strcpy(sv->option_m,"4096"); }
    if(v->Filebufmax >= 16384) { v->Ksize = 8192; strcpy(sv->option_m,"8192"); }
/*    v->Ksize = atol(sv->option_m);  */
  }

  /* If framelength was intended to match buffer size, stay in sync */
  v->Tframlen = atol (sv->option_f);
  if (v->Tframlen && v->Tframlen == origbuf)
    v->Tframlen = v->Filebufmax;

  v->ErrorLimit = atol (sv->option_e);

  /* Copy caller's io struct into our Vars for easier passing */
  v->io = *io;

#ifdef DEBUGLOG
  if (!DebugLog)
    DebugLog = (char *) (*v->io.xpr_fopen) (DebugName, "w");
  dlog (v, "XPR_IO struct:\n");
  for (i = 0, lng = (long *) io; i < sizeof (struct XPR_IO) / 4; ++i)
  {
    xprsprintf (v->Msgbuf, "  %08lx\n", *lng++);
    dlog (v, v->Msgbuf);
  }
  D (DEBUGINFO);
#endif

  /* Get baud rate; set serial port mode if necessary (and possible) */
  if (v->io.xpr_setserial)
  {
    v->Oldstatus = (*v->io.xpr_setserial) (-1L);
    if (v->Oldstatus != -1)
    {
      /*
         * ZModem requires 8 data bits, no parity (full transparency),
         *  leave other settings alone
       */
      newstatus = v->Oldstatus & 0xFFFFE0BC;
      /*
         * newstatus |= on_flags; Here's where we'd turn bits on if we
         * needed to
       */
      if (newstatus != v->Oldstatus)
        (*v->io.xpr_setserial) (newstatus);
      v->Baud = bauds[(newstatus >> 16) & 0xFF];

#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf,
                  "Old serial status = %lx, new = %lx, baud = %ld\n",
                  v->Oldstatus, newstatus, v->Baud);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
    }
    else
      v->Baud = 2400;
      /* If no xpr_setserial(), muddle along with most likely guess */
  }
  else
    v->Baud = 2400;

  v->FTNmode = (*sv->option_z == 'Y') ? TRUE : FALSE;
  v->dzap = (*sv->option_q == 'Y') ? TRUE : FALSE;
  v->NoFiles = (*sv->option_n == 'Y') ? TRUE : FALSE;
  v->SmallBlocks = (*sv->option_m == 1024) ? TRUE : FALSE;
  v->NewBaud = atol(sv->option_c);
  v->NoMask = (*sv->option_y == 'N') ? TRUE : FALSE; 
  return v;
}                                /* End of struct Vars *setup() */

/**********************************************************
 *      void set_textmode(struct Vars *v)
 *
 * Set text/binary mode flags in accordance with T option
 * setting
 **********************************************************/
void
set_textmode (struct Vars *v)
{
  struct SetupVars *sv;
  long i;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  sv = (void *) v->io.xpr_data;
  switch (*sv->option_t)
    {
    case 'Y':  /* Force text mode on receive; suggest text mode on send */
    TY:
      v->Rxascii = TRUE;
      v->Rxbinary = FALSE;
      v->Lzconv = ZCNL;
      break;
    case 'N': /* Force binary mode on receive; suggest binary mode on send */
    TN:
      v->Rxascii = FALSE;
      v->Rxbinary = TRUE;
      v->Lzconv = ZCBIN;
      break;
    case 'C':                        /* Ask comm program for proper mode for this file */
      if (v->io.xpr_finfo)
        {
          i = (*v->io.xpr_finfo) (v->Filename, 2L);
          if (i == 1)                /* Comm program says use binary mode */
            goto TN;
          if (i == 2)                /* Comm program says use text mode */
            goto TY;
        }
      /* xpr_finfo() not provided (or failed); default to T? */
    case '?':
      v->Rxascii = v->Rxbinary = FALSE;
      v->Lzconv = 0;
      break;
    }
}                                /* End of void set_textmode() */

/**********************************************************
 *      UBYTE *find_option(UBYTE *buf, UBYTE option)
 *
 * Search for specified option setting in string
 **********************************************************/
UBYTE *
find_option (UBYTE * buf, UBYTE option)
{
  while (*buf)
    {
      buf += strspn (buf, " ,\t\r\n");
      if (*buf == option)
        return ++buf;
      buf += strcspn (buf, " ,\t\r\n");
    }

  return NULL;
}                                /* End of UBYTE *find_option() */

/**********************************************************
 *      void canit(struct Vars *v)
 *
 * send cancel string to get the other end to shut up
 **********************************************************/
void
canit (struct Vars *v)
{
  static char canistr[] =
  {
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0
  };

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  zmputs (v, canistr);
}                                /* End of void canit() */

/**********************************************************
 *      void zmputs(struct Vars *v, UBYTE *s)
 *
 * Send a string to the modem, with processing for \336 (sleep 1 sec)
 * and \335 (break signal, ignored since XPR spec doesn't support it)
 **********************************************************/
void
zmputs (struct Vars *v, UBYTE * s)
{
  UBYTE c;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  while (*s)
    {
      switch (c = *s++)
        {
        case '\336':
#ifdef zedzap
          XprTimeOut(50L);
#else
          Delay (50L);  
#endif
        case '\335':
          break;
        default:
          sendline (v, c);
        }
    }
  sendbuf (v);
}                                /* End of void zmputs() */

/**********************************************************
 *      void xsendline(struct Vars *v, UBYTE c)
 *
 * Write one character to the modem
 **********************************************************/
void
xsendline (struct Vars *v, UBYTE c)
{
  v->Outbuf[v->Outbuflen++] = c;
  if (v->Outbuflen >= sizeof (v->Outbuf))
    sendbuf (v);

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

}                                /* End of void xsendline() */

/**********************************************************
 *      void sendbuf(struct Vars *v)
 *
 * Send any data waiting in modem output buffer
 **********************************************************/
void
sendbuf (struct Vars *v)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->Outbuflen)
    {
      (*v->io.xpr_swrite) (v->Outbuf, (long) v->Outbuflen);
      v->Outbuflen = 0;
    }
}                                /* End of void sendbuf() */

/**********************************************************
 *      short readock(struct Vars *v, short tenths)
 *
 * Get a byte from the modem;
 * return TIMEOUT if no read within timeout tenths of a
 * second, return RCDO if carrier lost or other fatal error
 * (sread returns -1).  Added in some buffering so we
 * wouldn't hammer the system with single-byte serial port
 * reads.  Also, the buffering makes char_avail() a lot
 * easier to implement.
 **********************************************************/
short
readock (struct Vars *v, short tenths)
{
  long t;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* If there's data waiting in our buffer, return next byte */
  if (v->Modemcount)
    {
    gotdata:
      --v->Modemcount;
      return (short) (*v->Modemchar++);
    }
  /*
     * Our buffer is empty; see if there's anything waiting in system buffer.
     * If the caller is in a hurry, don't wait around, but if it can spare
     * a half second, wait a bit and build up some input so we don't do as
     * many sread() calls.
   */
  t = (tenths < 5) ? 0 : 500000;

#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf,
              "Input buffer empty; calling sread for %ld bytes, %ld usec\n",
              (long) sizeof (v->Modembuf), t);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif
  v->Modemcount = (*v->io.xpr_sread) (v->Modembuf, (long) sizeof (v->Modembuf), t);

#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "   sread returned %ld\n", v->Modemcount);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif
  if (v->Modemcount < 0)        /* Carrier dropped or other fatal error; abort */
    {
      v->Modemcount = 0;
      return RCDO;
    }
  else if (!v->Modemcount)        /* Nothing in system buffer; try waiting */
    {
      t = tenths * 100000L - t;
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "   calling sread for 1 byte, %ld usec\n", t);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      v->Modemcount = (*v->io.xpr_sread) (v->Modembuf, 1L, t);
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "   sread returned %ld\n", v->Modemcount);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      if (v->Modemcount < 0)
        {
          v->Modemcount = 0;
          return RCDO;
        }
      else if (!v->Modemcount)        /* Nothing received in time */
        return TIMEOUT;
    }
  v->Modemchar = v->Modembuf;        /* Reset buffer pointer to start of data */
  goto gotdata;
}                                /* End of short readock() */

/**********************************************************
 *      char char_avail(struct Vars *v)
 *
 * Check if there's anything available to read from the
 * modem
 **********************************************************/
char
char_avail (struct Vars *v)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->Modemcount)
    return TRUE;

  /* No data in our buffer; check system's input buffer */
  v->Modemcount = (*v->io.xpr_sread)
    (v->Modembuf, (long) sizeof (v->Modembuf), 0L);
  if (v->Modemcount < 1)        /* Nothing in system buffer either */
    {
      v->Modemcount = 0;
      return FALSE;
    }
  else
    /* System buffer had something waiting for us */
    {
      v->Modemchar = v->Modembuf;
      return TRUE;
    }
}                                /* End of char char_avail() */

/**********************************************************
 *      void update_rate(struct Vars *v)
 *
 * Update the elapsed time, expected total time, and
 * effective data transfer rate values for status display
 **********************************************************/
void
update_rate (struct Vars *v)
{
  ULONG sent, elapsed, expect;
  short hr, min;
  struct timeval tv;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Compute effective data rate so far in characters per second */
  sent = v->xpru.xpru_bytes - v->Strtpos;
  getsystime (&tv);
  elapsed = (tv.tv_secs & 0x7FFFFF) * 128 + tv.tv_micro / 8192;
  elapsed -= (v->Starttime.tv_secs & 0x7FFFFF) * 128 + v->Starttime.tv_micro / 8192;
  if (elapsed < 128 || elapsed > 0x7FFFFF)
    elapsed = 128;
  /*
     * If we haven't transferred anything yet (just starting), make reasonable
     * guess (95% throughput); otherwise, compute actual effective transfer
     * rate
   */

  if (v->NewBaud == 0)
    v->xpru.xpru_datarate = (sent) ? (sent * 128 / elapsed) : (v->Baud * 95 / 1000);
  else
    v->xpru.xpru_datarate = (sent) ? (sent * 128 / elapsed) : (v->NewBaud * 95 / 1000);

  /* Compute expected total transfer time based on data rate so far */
  if (v->xpru.xpru_filesize < 0)
    expect = 1;                /* Don't know filesize; display time=1; was 0 thas to nice for GURU 8000 0005 */
  else
  {
   if((v->xpru.xpru_datarate != 0) && ((v->xpru.xpru_filesize - v->Strtpos) != 0))
      expect = (v->xpru.xpru_filesize - v->Strtpos) / v->xpru.xpru_datarate;
    else
      expect = 1; /* Display 1 secs better than nothing... */
  }

  hr = expect / 3600;                /* How many whole hours */
  expect -= hr * 3600;                /* Remainder not counting hours */
  min = expect / 60;                /* How many whole minutes */
  expect -= min * 60;                /* Remaining seconds */
  xprsprintf (v->Msgbuf, "%02ld:%02ld:%02ld", (long) hr, (long) min, expect);
  v->xpru.xpru_expecttime = (char *) v->Msgbuf;

  /* Compute elapsed time for this transfer so far */
  elapsed /= 128;
  hr = elapsed / 3600;
  elapsed -= hr * 3600;
  min = elapsed / 60;
  elapsed -= min * 60;
  xprsprintf (v->Msgbuf + 20, "%02ld:%02ld:%02ld", (long) hr, (long) min,
              elapsed);
  v->xpru.xpru_elapsedtime = (char *) v->Msgbuf + 20;
}                                /* End of void update_rate() */

/**********************************************************
 *      long bfopen(struct Vars *v, UBYTE *mode)
 *
 * Buffered file I/O fopen() interface routine
 **********************************************************/
long
bfopen (struct Vars *v, UBYTE * mode)
{
  /* Initialize file-handling variables */
  v->Filebufpos = v->Filebuflen = v->Filebufcnt = 0;
  v->Fileflush = FALSE;
  v->Filebufptr = v->Filebuf;
  /* Open the file */
#ifdef DEBUGLOG
  xprsprintf (v->Msgbuf, "bfopen: %s %s\n", v->Filename, mode);
  dlog (v, v->Msgbuf);
  D (DEBUGINFO);
#endif
  return (*v->io.xpr_fopen) (v->Filename, mode);
}                                /* End of long bfopen() */

/**********************************************************
 *      void bfclose(struct Vars *v)
 *
 * Buffered file I/O fclose() interface routine
 **********************************************************/
void
bfclose (struct Vars *v)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->File)
    {
      /* If bfwrite() left data in buffer, flush it out before closing */
      if (v->Fileflush)
        (*v->io.xpr_fwrite) (v->Filebuf, 1L, v->Filebufcnt, v->File);
      /* Close the file */
      (*v->io.xpr_fclose) (v->File);
      v->File = NULL;
    }
}                                /* End of void bfclose() */

/**********************************************************
 *      void bfseek(struct Vars *v, long pos)
 *
 * Buffered file I/O fseek() interface routine
 **********************************************************/
void
bfseek (struct Vars *v, long pos)
{
  long offset;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* If new file position is within currently buffered section,
     reset pointers */
  if (pos >= v->Filebufpos && pos < v->Filebufpos + v->Filebuflen)
    {
      offset = pos - v->Filebufpos;
      v->Filebufptr = v->Filebuf + offset;
      v->Filebufcnt = v->Filebuflen - offset;
      /* Otherwise, fseek() file & discard buffer contents to force new read */
    }
  else
    {
      (*v->io.xpr_fseek) (v->File, pos, 0L);
      v->Filebuflen = v->Filebufcnt = 0;
      v->Filebufpos = pos;
    }
}                                /* End of void bfseek() */

/**********************************************************
 *      long bfread(struct Vars *v, UBYTE *buf, long length)
 *
 * Buffered file I/O fread() interface routine
 **********************************************************/
long
bfread (struct Vars *v, UBYTE * buf, long length)
{
  long count, total = 0;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Keep going until entire request completed */
  while (length > 0)
    {
      /* Copy as much of the request as possible from the buffer */
      count = (length <= v->Filebufcnt) ? length : v->Filebufcnt;
      CopyMem (v->Filebufptr, buf, count);
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "bfread got %ld bytes from buffer\n", count);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      buf += count;
      total += count;
      length -= count;
      v->Filebufptr += count;
      v->Filebufcnt -= count;

      /* If we've emptied the buffer, read next buffer's worth */
      if (!v->Filebufcnt)
        {
          v->Filebufpos += v->Filebuflen;
          v->Filebufptr = v->Filebuf;
          v->Filebufcnt = v->Filebuflen
            = (*v->io.xpr_fread) (v->Filebuf, 1L, v->Filebufmax, v->File);
#ifdef DEBUGLOG
          xprsprintf (v->Msgbuf, "bfread read %ld bytes\n", v->Filebufcnt);
          dlog (v, v->Msgbuf);
          D (DEBUGINFO);
#endif
          /* If we hit the EOF, return with however much we read so far */
          if (!v->Filebufcnt)
            break;
        }
    }
  return total;
}                                /* End of long bfread() */

/**********************************************************
 *      long bfwrite(struct Vars *v, UBYTE *buf, long length)
 *
 * Buffered file I/O fwrite() interface routine
 **********************************************************/
long
bfwrite (struct Vars *v, UBYTE * buf, long length)
{
  long count, total = 0;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  /* Keep going until entire request completed */
  while (length > 0)
    {
      /* Copy as much as will fit into the buffer */
      count = v->Filebufmax - v->Filebufcnt;
      if (length < count)
        count = length;
      CopyMem (buf, v->Filebufptr, count);
#ifdef DEBUGLOG
      xprsprintf (v->Msgbuf, "bfwrite buffered %ld bytes\n", count);
      dlog (v, v->Msgbuf);
      D (DEBUGINFO);
#endif
      buf += count;
      total += count;
      length -= count;
      v->Filebufptr += count;
      v->Filebufcnt += count;
      v->Fileflush = TRUE;

      /* If we've filled the buffer, write it out */
      if (v->Filebufcnt == v->Filebufmax)
      {
        count = (*v->io.xpr_fwrite) (v->Filebuf, 1L, v->Filebufcnt, v->File);
#ifdef DEBUGLOG
xprsprintf (v->Msgbuf, "bfwrite wrote %ld bytes\n", count);
dlog (v, v->Msgbuf);
D (DEBUGINFO);
#endif
        if (count < v->Filebufcnt)
          return -1;
        v->Filebufptr = v->Filebuf;
        v->Filebufcnt = 0;
        v->Fileflush = FALSE;
      }
    }
  return total;
}                                /* End of long bfwrite() */

/**********************************************************
 *      void ioerr(struct XPR_IO *io, char *msg)
 *
 * Have the comm program display an error message for us,
 * using a temporary XPR_UPDATE structure; used to display
 * errors before Vars gets allocated
 **********************************************************/
void
ioerr (struct XPR_IO *io, char *msg)
{
  struct XPR_UPDATE xpru;

  if (io->xpr_update)
    {
      xpru.xpru_updatemask = XPRU_ERRORMSG;
      xpru.xpru_errormsg = msg;
      (*io->xpr_update) (&xpru);
    }
}                                /* End of void ioerr() */

/**********************************************************
 *     void updstatus(struct Vars *v,char *filename,long status)
 *
 * Set the pass/fail status of this file
 * Have the comm program display the file status for us, using the
 * normal XPR_IO structure allocated in Vars
 **********************************************************/
void
updstatus (struct Vars *v, char *filename, long status, long mask)
{
  if (v->NoMask) mask=0;
  v->xpru.xpru_updatemask = XPRU_FILENAME | XPRU_STATUS | mask;
  v->xpru.xpru_filename = filename;
  v->xpru.xpru_status = status;
  (*v->io.xpr_update) (&v->xpru);
}

/**********************************************************
 *      void upderr(struct Vars *v, char *msg)
 *
 * Have the comm program display an error message for us, using the
 * normal XPR_IO structure allocated in Vars
 **********************************************************/
void
upderr (struct Vars *v, char *msg, long mask)
{
  if (v->NoMask) mask=0;
  v->xpru.xpru_updatemask = XPRU_ERRORMSG | mask;
  v->xpru.xpru_errormsg = msg;
  if (msg == v->Msgbuf)                /* Ensure message length < 50 */
    msg[48] = '\0';
  (*v->io.xpr_update) (&v->xpru);
#ifdef DEBUGLOG
  dlog (v, msg);
  dlog (v, "\n");
  D (DEBUGINFO);
#endif
}                                /* End of void upderr() */

/**********************************************************
 *      void updmsg(struct Vars *v,char *msg)
 *
 * Have the comm program display a normal message for us
 **********************************************************/
void
updmsg (struct Vars *v, char *msg, long mask)
{
#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  if (v->NoMask) mask=0;
  v->xpru.xpru_updatemask = XPRU_MSG | mask;
  v->xpru.xpru_msg = msg;
  if (msg == v->Msgbuf)                /* Ensure message length < 50 */
    msg[48] = '\0';
  (*v->io.xpr_update) (&v->xpru);
#ifdef DEBUGLOG
  dlog (v, msg);
  dlog (v, "\n");
  D (DEBUGINFO);
#endif
}                                /* End of void updmsg() */

/**********************************************************
 *      long getfree(void)
 *
  * Figure out how many bytes are free on the drive we're uploading to.
 * Stubbed out for now; not supported by XPR spec.
 **********************************************************/
long
getfree (void)
{
  return 0x7FFFFFFF;
}                                /* End of long getfree() */

/**********************************************************
 *      char exist(struct Vars *v)
 *
 * Check whether file already exists; used to detect
 * potential overwrites
 **********************************************************/
char
exist (struct Vars *v)
{
  long file;

#ifdef DEBUGLOG
  D (DEBUGINFO);
#endif

  file = (*v->io.xpr_fopen) (v->Filename, "r");
  if (file)
  {
      (*v->io.xpr_fclose) (file);
      return TRUE;
  }
  else
  {
    return FALSE;
  }
}                                /* End of char exist() */

#ifdef DEBUGLOG
/**********************************************************
 *      void dlog(struct Vars *v, UBYTE *s)
 *
 * Write a message to the debug log
 **********************************************************/
void
dlog (struct Vars *v, UBYTE * s)
{
  /* Open the debug log if it isn't already open */
  if (!DebugLog)
    DebugLog = (char *) (*v->io.xpr_fopen) (DebugName, "a");
  (*v->io.xpr_fwrite) (s, 1L, (long) strlen (s), (long) DebugLog);
  /*
     * Close file to flush output buffer; comment these two lines out if
     * you aren't crashing your system and don't mind waiting until the
     * transfer finishes to look at your log file.
   */
  /* (*v->io.xpr_fclose)((long)DebugLog);
     * DebugLog = NULL;
   */

}                                /* End of void dlog() */
#endif
/* End of Utils.c source */
