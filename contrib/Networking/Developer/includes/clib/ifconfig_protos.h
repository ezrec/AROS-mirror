#ifndef CLIB_IFCONFIG_PROTOS_H
#define CLIB_IFCONFIG_PROTOS_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef LIBRARIES_IFCONFIG_H
#include <libraries/ifconfig.h>
#endif

/* ifconfig.library functions */

/*
 * Reserve the first entry for possible ARexx interface
 */
LONG Reserved(void);

/*
 * Find the interface configuration for interface ifname.
 */
LONG IfConfigFind(const char *ifname, struct ifconfig ** ifcp);

/*
 * Free the configuration got from IfConfigFind().
 */
void IfConfigFree(struct ifconfig * config);

/*
 * Find all interface configurations matching the filter. filter can be NULL. 
 */
LONG IfConfigFindAll(struct List ** ifclist, struct TagItem * filter);

/*
 * Free a list of configuration entries returned by IfConfigFindAll().
 */
void IfConfigFreeList(struct List * list);

/*
 * Convert ifconfig.library error codes to english language error strings.
 */
const UBYTE * IfConfigStrError(ULONG ioerr, char * buf, ULONG bufsiz);

/*
 * Return (english) name of the tag
 */
const struct ifc_confitem * IfConfigGetTagInfo(ULONG tagcode);

#endif /* CLIB_IFCONFIG_PROTOS_H */
