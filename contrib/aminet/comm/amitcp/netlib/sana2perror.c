/*
 * sana2perror.c --- print SANA-II error message
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sat Mar 20 02:10:14 1993 ppessi
 * Last modified: Thu Mar 24 07:19:51 1994 ppessi
 */
#define KERNEL 1
#ifdef KERNEL
/*
 * Note: This file is to be recompiled with AmiTCP/IP proper
 *       with preprocessor symbol KERNEL defined.
 *       You should NOT link it directly from "net.lib"! 
 */
#include <conf.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/syslog.h>
#define fprintf log
#define stderr LOG_ERR
#else
#include <stdio.h>
#endif

#include <exec/types.h>
#include <devices/sana2.h>
#include <net/sana2errno.h>

/****** sana2.lib/sana2perror *************************************************

    NAME
	sana2perror - print SANA-II device error messages

    SYNOPSIS
	#include <devices/sana2.h>

	sana2perror(banner, ios2request)

	void sana2perror(const char *, struct IOSana2Req *)

    FUNCTION
	The sana2perror() function finds the error message corresponding to
	the error in the given SANA-II IO request and writes it, followed by a
	newline, to the stderr.  If the argument string is non-NULL it is
	preappended to the message string and separated from it by a colon and
	space (`: ').  If string is NULL only the error message string is
	printed.

    NOTES
	The sana2perror() function requires the stdio functions to be linked.

    SEE ALSO
	Sana2PrintFault()

*******************************************************************************
*/

void 
sana2perror(const char *banner, struct IOSana2Req *ios2)
{
  register WORD err = ios2->ios2_Req.io_Error;
  register ULONG werr = ios2->ios2_WireError;
  const char *errstr;

  if (err >= sana2io_nerr || -err > io_nerr) {
    errstr = io_errlist[0];
  } else { 
    if (err < 0) 
      /* Negative error codes are common with all IO devices */
      errstr = io_errlist[-err];
    else 
      /* Positive error codes are SANA-II specific */ 
      errstr = sana2io_errlist[err];
  }

  if (werr == 0 || werr >= sana2wire_nerr) {
    if (banner != NULL)
      fprintf(stderr, "%s: %s\n", banner, errstr);
    else
      fprintf(stderr, "%s\n", errstr);
  } else {
    if (banner != NULL)
      fprintf(stderr, "%s: %s (%s)\n", banner, errstr, sana2wire_errlist[werr]);
    else
      fprintf(stderr, "%s (%s)\n", errstr, sana2wire_errlist[werr]);
  }
}

