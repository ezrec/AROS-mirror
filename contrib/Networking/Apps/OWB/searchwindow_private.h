#ifndef _SEARCHWINDOW_PRIVATE_H_
#define _SEARCHWINDOW_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct SearchWindow_DATA
{
    Object *located_string;
    Object *next_button;
    Object *previous_button;
    Object *case_checkmark;
    struct Hook *search_hook;
};

#endif /* _SEARCHWINDOW_PRIVATE_H_ */
