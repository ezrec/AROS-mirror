/*
 * $Id$
 *
 * Copyright (c) 1993, 1994	AmiTCP/IP Group, <amitcp-group@hut.fi>,
 *                    		Helsinki University of Technology, Finland.
 *                    		All rights reserved.
 *
 * Created: Tue Mar 22 22:17:57 1994 too
 * Last modified: Fri Mar 25 20:39:11 1994 too
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 3.1  1994/03/26  09:39:45  too
 * Initial release
 *
 */
#ifndef KERN_ACCESSCONTROL_H
#define KERN_ACCESSCONTROL_H

#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _NETINET_IN_H_
#include <netinet/in.h>
#endif

#ifndef KERN_AMIGA_NETDB_H
#include <kern/amiga_netdb.h>
#endif


int controlaccess(struct in_addr shost, unsigned short sport);

static inline void setup_accesscontroltable(struct NetDataBase * ndb)
{
  *((ULONG *)&ndb->ndb_AccessTable[ndb->ndb_AccessCount]) = 0; /*mark default*/

  ndb->ndb_AccessTable =
    bsd_realloc(ndb->ndb_AccessTable,
		ndb->ndb_AccessCount * sizeof (struct AccessItem) +
		sizeof (ULONG), M_NETDB, M_WAITOK);

#if 0
  {
    int i;
#define host ndb->ndb_AccessTable[i].ai_host
#define mask ndb->ndb_AccessTable[i].ai_mask
    for (i = 0; i < ndb->ndb_AccessCount; i++)
      log(7, "%ld %ld.%ld.%ld.%ld/%ld.%ld.%ld.%ld %lx",
	  ndb->ndb_AccessTable[i].ai_port, 
	  host>>24 & 0xff, host>>16 & 0xff, host>>8 & 0xff, host & 0xff,
	  mask>>24 & 0xff, mask>>16 & 0xff, mask>>8 & 0xff, mask & 0xff,
	  ndb->ndb_AccessTable[i].ai_flags);
    
    log(7, "%ld %ld", ndb->ndb_AccessTable[i].ai_flags,
	/*                 */ ndb->ndb_AccessTable[i].ai_port);
#undef mask
#undef host    
  }
#endif	  
}
#endif /* ACCESSCONTROL_H */

#endif /* KERN_ACCESSCONTROL_H */
