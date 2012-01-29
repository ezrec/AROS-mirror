#ifndef CLIB_MUIBUILDER_PROTOS_H
#define CLIB_MUIBUILDER_PROTOS_H


/*
**	$VER: muibuilder_protos.h 1.0 (03.01.2011)
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2011 MUIBuilder Open Source Team
**	All Rights Reserved
*/

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

BOOL MB_Open(void);
void MB_Close(void);
void MB_GetA(struct TagItem * TagList);
void MB_Get(Tag TagList, ...);
void MB_GetVarInfoA(ULONG varnb, struct TagItem * TagList);
void MB_GetVarInfo(ULONG varnb, Tag TagList, ...);
void MB_GetNextCode(ULONG * type, char ** code);
void MB_GetNextNotify(ULONG * type, char ** code);

#endif	/*  CLIB_MUIBUILDER_PROTOS_H  */
