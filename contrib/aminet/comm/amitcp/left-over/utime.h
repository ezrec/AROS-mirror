#ifndef	UTIME_H
#define	UTIME_H
/*
 * $Id$
 *
 * definitions and prototype for the utime() (in the net.lib)
 *
 * Copyright © 1994 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 */

struct utimbuf {
	time_t actime;		/* Access time */
	time_t modtime;		/* Modification time */
};

int utime(const char *, const struct utimbuf *);

#endif /* !UTIME_H */
