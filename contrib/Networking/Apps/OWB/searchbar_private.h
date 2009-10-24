#ifndef _SEARCHBAR_PRIVATE_H_
#define _SEARCHBAR_PRIVATE_H_

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/hooks.h>
#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct SearchBar_DATA
{
    Object *located_string;
    Object *next_button;
    Object *previous_button;
    Object *case_checkmark;
};

#endif /* _SEARCHBAR_PRIVATE_H_ */
