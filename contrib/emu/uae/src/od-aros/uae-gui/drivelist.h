#ifndef _DRIVELIST_H_
#define _DRIVELIST_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>

/*** Variables **************************************************************/
extern struct MUI_CustomClass *DriveList_CLASS;

/*** Macros *****************************************************************/
#define DriveListObject BOOPSIOBJMACRO_START(DriveList_CLASS->mcc_Class)

#endif /* _DRIVELIST_H_ */
