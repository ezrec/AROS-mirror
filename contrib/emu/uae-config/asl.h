#ifndef _ASL_H_
#define _ASL_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>

/*** Modes ******************************************************************/
enum ASLMode
{
    AM_LOAD,
    AM_SAVE
};

/*** Prototypes *************************************************************/
STRPTR ASL_SelectFile();

#endif /* _ASL_H_ */
