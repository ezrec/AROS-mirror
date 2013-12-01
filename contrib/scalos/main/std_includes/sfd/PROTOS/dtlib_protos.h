#ifndef  CLIB_DTLIB_PROTOS_H
#define  CLIB_DTLIB_PROTOS_H

/*
**	$Id$
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
Class *ObtainInfoEngine( struct Library *libBase );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_DTLIB_PROTOS_H */
