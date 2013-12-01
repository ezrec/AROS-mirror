#ifndef  CLIB_SCALOSFILETYPEPLUGIN_PROTOS_H
#define  CLIB_SCALOSFILETYPEPLUGIN_PROTOS_H

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

#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif
STRPTR SCAToolTipInfoString( struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR args );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_SCALOSFILETYPEPLUGIN_PROTOS_H */
