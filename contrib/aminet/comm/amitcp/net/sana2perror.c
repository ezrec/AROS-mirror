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
 * Last modified: Mon Jan 24 00:00:44 1994 jraja
 */

#include <conf.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/syslog.h>

#include <devices/sana2.h>
#include <net/sana2errno.h>

extern const char * const io_errlist[];
extern const short io_nerr;
extern const char * const sana2io_errlist[];
extern const short sana2io_nerr;
extern const char * const sana2wire_errlist[];
extern const short sana2wire_nerr;

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
    log(LOG_ERR, "%s: %s\n", banner, errstr);
  } else {
    log(LOG_ERR, "%s: %s (%s)\n", banner, errstr, sana2wire_errlist[werr]);
  }
}

