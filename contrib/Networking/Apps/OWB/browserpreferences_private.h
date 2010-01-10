#ifndef _BROWSERPREFERENCES_PRIVATE_H_
#define _BROWSERPREFERENCES_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>

/*** Instance data **********************************************************/
struct BrowserPreferences_DATA
{
    STRPTR downloadDestination;
    BOOL requestDownloadedFileName;
};

#endif /* _BROWSERPREFERENCES_PRIVATE_H_ */
