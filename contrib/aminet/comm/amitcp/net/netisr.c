/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>,
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * netisr.c - Network protocol input queue scheduling
 *
 * Created      : Sat Feb 20 04:13:15 1993 ppessi
 * Last modified: Fri Jun  4 00:38:31 1993 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.8  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.7  1993/05/16  21:09:43  ppessi
 * RCS version changed.
 *
 * Revision 1.6  1993/04/05  17:46:05  jraja
 * Changed spl storage variables to spl_t.
 * Changed every .c file to use conf.h.
 *
 * Revision 1.5  93/03/05  19:50:52  19:50:52  jraja (Jarno Tapio Rajahalme)
 * Fixed includes (again).
 * 
 * Revision 1.4  93/03/05  03:12:29  03:12:29  ppessi (Pekka Pessi)
 * Compiles with SASC. Initial test version
 * 
 * Revision 1.3  93/02/28  21:44:31  21:44:31  ppessi (Pekka Pessi)
 * 	Revised version, added non-signalling schednetisr_nosignal.
 * 
 */

#include <conf.h>

#include <sys/param.h>
#include <kern/amiga_includes.h>	/* This is needed by sys/synch.h */
#include <sys/synch.h>

#include <net/netisr.h>


extern struct MsgPort *SanaPort;

ULONG netisr = 0L;

/*
 * scnednetisr(): schedule net "interrupt" 
 * This routine signals TCP/IP task to run net_poll via sana_poll
 */
void schednetisr(int isr)
{
  netisr |= 1<<isr;
  Signal(SanaPort->mp_SigTask, 1<<SanaPort->mp_SigBit);
}

void schednetisr_nosignal(int isr)
{
  netisr |= 1<<isr;
}

/* 
 * net_poll(): run scheduled network level protocols
 *             this routine is called from sana_poll
 */
void 
  net_poll(void)
{
  extern void ipintr(void);
  extern void impintr(void);
  extern void isointr(void);
  extern void ccittintr(void);
  extern void nsintr(void);

  spl_t s = splimp();
  int n = netisr; 
  netisr = 0;
  splx(s);

  if (!n) return;
#if	INET
  if (n & (1<<NETISR_IP)) {
    ipintr();
  }
#endif /* INET */
#if	NIMP > 0
  if (n & (1<<NETISR_IMP)) {
    impintr();
  }
#endif /* NIMP > 0 */
#if	ISO
  if (n & (1<<NETISR_ISO)) {
    isointr();
  }
#endif /* ISO */
#if	CCITT
  if (n & (1<<NETISR_CCITT)) {
    ccittintr();
  }
#endif /* CCITT */
#if	NS
  if (n & (1<<NETISR_NS)) {
    nsintr();
  }
#endif /* NS */
#if 0
  /* raw input do not go through the isr */
  if (n & (1<<NETISR_RAW)) {
    rawintr();
  }
#endif
}
