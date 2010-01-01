#ifndef _BROWSERAPP_PRIVATE_H_
#define _BROWSERAPP_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct BrowserApp_DATA
{
    Object *wnd;
    Object *preferencesManager;
    Object *preferences;
    Object *bookmarkManager;
    Object *downloadManager;
    struct Hook forwardCallToActiveWindowHook;
    struct Hook requestFileHook;
    struct Hook closeWindowHook;
};

#endif /* _BROWSERAPP_PRIVATE_H_ */
