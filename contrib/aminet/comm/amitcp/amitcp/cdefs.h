/*
 * $Id$
 *
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:17:12  henrik
 * *** empty log message ***
 *
 * Revision 1.12  1994/03/22  07:18:15  jraja
 * Added compiler dependent definitions ASM and REG(x).
 *
 * Revision 1.11  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.10  1993/05/17  01:02:04  ppessi
 * Changed RCS version
 *
 * Revision 1.9  1993/05/05  16:10:47  puhuri
 * Fixes for final demo.
 *
 * Revision 1.8  93/04/13  21:56:30  21:56:30  jraja (Jarno Tapio Rajahalme)
 * Added new keyword: ALIGNED.
 * 
 * Revision 1.7  93/04/11  22:21:36  22:21:36  jraja (Jarno Tapio Rajahalme)
 * Added function modifier STKARGFUN to be used with functions whose arguments
 * MUST be passed in stack.
 * 
 * Revision 1.6  93/03/07  00:57:12  00:57:12  jraja (Jarno Tapio Rajahalme)
 * Added definition for REGARGFUN keyword.
 * 
 * Revision 1.5  93/03/03  20:08:54  20:08:54  jraja (Jarno Tapio Rajahalme)
 * Removed redundant copyright message.
 * 
 * Revision 1.4  93/03/03  20:06:32  20:06:32  jraja (Jarno Tapio Rajahalme)
 * Cleanup. Changed _CDEFS_H_ to SYS_CDEFS_H.
 * 
 * Revision 1.3  93/03/03  12:32:04  12:32:04  jraja (Jarno Tapio Rajahalme)
 * Fixed _SASC_60 to __SASC_60.
 * Added SAVEDS definition.
 * 
 * Revision 1.2  92/12/22  00:26:32  00:26:32  jraja (Jarno Tapio Rajahalme)
 * Added trailing underscores to __GNUC__ keywords and SASC6.0 inline
 * definition
 * 
 * Revision 1.1  92/11/20  15:41:43  15:41:43  jraja (Jarno Tapio Rajahalme)
 * Initial revision
 * 
 *
 */

/* Since all the standard declarations are in ixemul's sys/cdefs.h,
 * I have deleted them from here, leaving only AmiTCP specific 
 * defines.  Hence, I removed the Berkeley copyrights as well.
 * L.W.
 */


#ifndef	AMITCP_CDEFS_H
#define	AMITCP_CDEFS_H

/*
 * SAVEDS should be used in all function definitions which will be called 
 * from other tasks than AmiTCP/IP. Is restores the global data base pointer
 * as the first thing in the function body.
 *
 * REGARGFUN contains special keywords which should be used when functions
 * used through shared library are referenced.
 */

#define SAVEDS __saveds
#define REGARGFUN __regargs
#define STKARGFUN __stdargs
#define ALIGNED __aligned
#define ASM __asm
#define REG(x) register __##x

#endif /* !AMITCP_CDEFS_H */
