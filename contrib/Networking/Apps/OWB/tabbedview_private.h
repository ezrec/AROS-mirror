#ifndef _TABBEDVIEW_PRIVATE_H_
#define _TABBEDVIEW_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct TabbedView_DATA
{
    Object *activeObject;
    struct Hook setterHook;
};

#endif /* _TABBEDVIEW_PRIVATE_H_ */
