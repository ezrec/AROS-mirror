#ifndef _LOCALE_H_
#define _LOCALE_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <aros/locale.h>

/*** Macros *****************************************************************/
#define _(x)   ((STRPTR)GetString(x)) /* Get a message, as an STRPTR */
#define __(id) ((IPTR) _(id))         /* Get a message, as an IPTR */

#endif /* _LOCALE_H_ */
