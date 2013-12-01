/*
 *  tcp_locale.h - The locale stuff not automatically created
 *
 * $Date$
 * $Revision$
 *
 *  40.1
 *  20010719 DM Created. Only contains function for reading locale strings from
 *              the CatCompArray and incldues the automatically created CatComp
 *              locale header file.
 *
 */

#ifndef TCP_LOCALE_H
#define TCP_LOCALE_H

#include <exec/types.h>

extern struct Catalog  *tcp_Catalog;

STRPTR GetLocString(ULONG msgid);

#define CATCOMP_NUMBERS
#include "cc_locale.h"

#endif /* TCP_LOCALE_H */

