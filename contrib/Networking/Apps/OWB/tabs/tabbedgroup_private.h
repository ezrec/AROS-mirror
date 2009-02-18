#ifndef _TABBEDGROUP_PRIVATE_H_
#define _TABBEDGROUP_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>

enum { HORIZONTAL_TABBED = 1, VERTICAL_TABBED };

/*** Instance data **********************************************************/
struct TabbedGroup_DATA
{
    BYTE direction;
};

#endif /* _TABBEDGROUP_PRIVATE_H_ */
