/*
 * $Id$
 *
 * Created: Tue May  4 11:01:40 1993 too
 * Last modified: Wed Jun  2 17:12:03 1993 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.2  2001/12/26 19:30:19  henrik
 * Aros version
 *
 * Revision 1.1  2001/12/25 22:21:39  henrik
 *
 * Revision 1.4  1994/03/22  07:33:28  jraja
 * Moved f_void definition from apicalls_sasc.h to apicalls.h.
 *
 * Revision 1.3  1993/06/02  14:13:22  jraja
 * Updated.
 *
 * Revision 1.2  1993/06/01  11:41:50  jraja
 * Moved apicalls.*\.h from sys to api.
 *
 * Revision 1.1  1993/05/04  14:09:34  too
 * Initial revision
 *
 */

#ifndef API_APICALLS_H
#define API_APICALLS_H

/* f_void either needs to be redefined, or whatever uses it.
   It's only used in 2 files - amiga_api.c and amiga_libtables.c,
   both in api subdir.
*/

typedef VOID (* REGARGFUN f_void)();

#if __SASC
#include <api/apicalls_sasc.h>
#elif __GNUC__
#include <api/apicalls_gnuc.h>
#else
#error "AmiTCP/IP internal API calls are not defined for your compiler!"
#endif

#endif /* API_APICALLS_H */
