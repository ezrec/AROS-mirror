#ifndef _SUPPORT_H_
#define _SUPPORT_H_

/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <exec/types.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
/*** Prototypes *************************************************************/
Object *MakeToggleButton(CONST_STRPTR label, CONST_STRPTR shortHelp);
Object *MakePushButton(CONST_STRPTR label, CONST_STRPTR shortHelp);

#endif /* _SUPPORT_H_ */
