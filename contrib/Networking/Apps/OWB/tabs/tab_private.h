#ifndef _TAB_PRIVATE_H_
#define _TAB_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct Tab_DATA
{
    WORD frameWidth;
    WORD frameHeight;
    BYTE active;
    LONG background;
    BYTE nodraw;
};

#endif /* _TAB_PRIVATE_H_ */
