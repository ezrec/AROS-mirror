#ifndef  CLIB_NEWICON_PROTOS_H
#define  CLIB_NEWICON_PROTOS_H

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
#ifndef  LIBRARIES_NEWICON_H
#include <libraries/newicon.h>
#endif
struct NewDiskObject *GetNewDiskObject( UBYTE *name );
BOOL PutNewDiskObject( UBYTE *name, struct NewDiskObject *newdiskobj );
VOID FreeNewDiskObject( struct NewDiskObject *newdiskobj );
BOOL newiconPrivate1( struct NewDiskObject *diskobj );
UBYTE **newiconPrivate2( struct NewDiskObject *diskobj );
VOID newiconPrivate3( struct NewDiskObject *diskobj );
struct Image *RemapChunkyImage( struct ChunkyImage *chunkyimage, struct Screen *screen );
VOID FreeRemappedImage( struct Image *image, struct Screen *screen );
struct NewDiskObject *GetDefNewDiskObject( LONG def_type );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_NEWICON_PROTOS_H */
