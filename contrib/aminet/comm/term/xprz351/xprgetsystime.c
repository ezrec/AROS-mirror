/**********************************************************************
 * Amiga-type replacement for standard Unix time() function.  Can't use
 * Lattice's time() function from within a library because it's not
 * reentrant (has some hidden static vars it sets) and because it wants
 * you to have opened dos.library for some reason, which is supposedly
 * a bad idea inside a library.  Oh, well... this is quite a bit
 * smaller & faster anyway.  B-)
 **********************************************************************/

#include "xprzmodem_all.h"

/*
 * # seconds between 1-1-70 (Unix time base) and 1-1-78 (Amiga time base).
 * Add this value to the returned seconds count to convert Amiga system time
 * to normal Unix system time.
 */

ULONG UnixTimeOffset = 252482400;

#ifdef zedzap
/**********************************************************
 *      ULONG getsystime(struct timeval *tv)
 *
 * This is the old function and is used when ZEDZAP is DEFINED
 **********************************************************
 *
 * Returns current system time in standard Amiga-style timeval
 * structure, if you pass a pointer to one.  Also returns the
 * seconds part as the return value.  This lets you get just
 * the seconds by calling getsystime(NULL) if that's all you
 * want, or the full timeval struct if you need that.  This
 * is very similar to how the standard time() function is used.
 *
 **********************************************************/
 ULONG
getsystime(struct timeval *tv)
 {
 struct timerequest tr;

 /*
 * timer.device must be working or the system would've died, so let's
 * not bother with error checking.
 */

 memset(&tr, 0, sizeof(tr));
 OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *) &tr, 0L);

 tr.tr_node.io_Message.mn_Node.ln_Type = NT_MESSAGE;
 tr.tr_node.io_Command = TR_GETSYSTIME;

 DoIO((struct IORequest *) &tr);

 if (tv)
 *tv = tr.tr_time;

 CloseDevice((struct IORequest *) &tr);

 return tr.tr_time.tv_secs;
 }      /* End of ULONG getsystime() */

#else 
/**********************************************************
 *      ULONG getsystime(struct timeval *tv)
 *
 *      This function was rewritten using DateStamp() to
 * eliminate the opening and closing of the timer.device
 * that was occurring everytime this function was called.
 * An attempt to save some processing time.   -WMP-
 **********************************************************/
ULONG 
getsystime (struct timeval *tv)
{
  struct DateStamp ds;
  ULONG secs;

  DateStamp (&ds);

  secs = (ds.ds_Days * 86400) + (ds.ds_Minute * 60) + (ds.ds_Tick / TICKS_PER_SECOND);
  if (tv)
  {
    tv->tv_secs = secs;
    tv->tv_micro = 0;
  }

  return secs;
}
#endif

/* End of getsystime.c source */
