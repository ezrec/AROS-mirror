#ifndef  CLIB_ICONOBJECT_PROTOS_H
#define  CLIB_ICONOBJECT_PROTOS_H

/*
**	$Id$
**
**	C prototypes. For use with 32 bit integers only.
**
**	©1999-2002 The Scalos Team
**	    All Rights Reserved
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef  WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
Object *NewIconObject( CONST_STRPTR name, CONST struct TagItem *tagList );
Object *NewIconObjectTags( CONST_STRPTR name, ULONG firstTag, ... );
VOID DisposeIconObject( Object *iconObject );
Object *GetDefIconObject( ULONG iconType, CONST struct TagItem *tagList );
Object *GetDefIconObjectTags( ULONG iconType, ULONG firstTag, ... );
LONG PutIconObject( Object *iconObject, CONST_STRPTR path, CONST struct TagItem *tagList );
LONG PutIconObjectTags( Object *iconObject, CONST_STRPTR path, ULONG firstTag, ... );
ULONG IsIconName( CONST_STRPTR fileName );
Object *Convert2IconObject( struct DiskObject *diskObject );
Object *Convert2IconObjectA( struct DiskObject *diskObject, CONST struct TagItem *tagList );
Object *Convert2IconObjectTags( struct DiskObject *diskObject, ULONG firstTag, ... );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_ICONOBJECT_PROTOS_H */
