/*Desparation separation
 *  What follows is extracted from resolv.h, to get rid of
 *  the circularity with amiga_api.h.  Blech!
 *    Don't include from anything else. (!)
 */
#ifndef API_RESOLV_EXTRAS_H
#define API_RESOLV_EXTRAS_H
#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif


struct state {
	int	retrans;	 	/* retransmition time interval */
	int	retry;			/* number of times to retransmit */
	long	options;		/* option flags - see below. */
	u_short	id;			/* current packet id */
};

#endif /*API_RESOLV_EXTRAS_H */
