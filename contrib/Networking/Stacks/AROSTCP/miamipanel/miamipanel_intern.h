/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: miamipanel_intern.h 15644 2002-10-30 17:36:30Z stegerg $
*/

#ifndef _MIAMIPANEL_INTERN_H
#define _MIAMIPANEL_INTERN_H

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef EXEC_MEMORY_H
#   include <exec/memory.h>
#endif
#ifndef UTILITY_UTILITY_H
#   include <utility/utility.h>
#endif



/****************************************************************************************/

struct MiamiPanelBase_intern
{
    struct Library		library;
    struct ExecBase		*sysbase;
    BPTR			seglist;
};

/****************************************************************************************/

#undef CIB
#define CIB(b)	((struct MiamiPanelBase_intern *)(b))

#undef SysBase
#define SysBase     	(CIB(MiamiPanelBase)->sysbase)


/****************************************************************************************/

#endif /* _MIAMIPANEL_INTERN_H */
