#ifndef _BROWSERTAB_PRIVATE_H_
#define _BROWSERTAB_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct BrowserTab_DATA
{
    STRPTR title;
    Object *titleObject;
};

#endif /* _BROWSERTAB_PRIVATE_H_ */
