/*
 * Author: Tomi Ollila <too@cs.hut.fi>
 *
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * Created: Mon May 10 10:40:20 1993 too
 * Last modified: Sat Apr  2 13:52:04 1994 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:21:39  henrik
 * lots of assembler in api/amiga_api.h that needs to be fixed
 * better not do it since my knowledge of assembler i almost zero
 * think one need to change adresses to something like eax,ebx or something?
 *
 * Revision 3.1  1994/04/02  11:06:28  jraja
 * Moved global resolver variables to SocketBase, removed res_lock.
 *
 * Revision 1.4  1994/01/20  02:16:05  jraja
 * Added #include <conf.h> as the first include.
 *
 * Revision 1.3  1993/06/03  19:05:48  too
 * Fixed global res_lock to be zero after compilation (for sure)
 *
 * Revision 1.2  1993/06/02  19:26:17  too
 * Moved resolver stuff here from kern/ -directory
 *
 * Revision 1.1  1993/06/01  16:30:33  too
 * Initial revision
 *
 */

#include <conf.h>

#include <sys/param.h>
#include <kern/amiga_includes.h>

#include <api/resolv.h>

#ifndef AMITCP /* AmiTCP has this in the SocketBase */
struct state _res;
#endif

void
res_init(struct state *state)
{
  state->retrans = RES_TIMEOUT;
  state->retry   = 4;
  state->options = RES_DEFAULT;
}

