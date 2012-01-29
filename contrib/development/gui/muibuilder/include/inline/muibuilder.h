#ifndef _INLINE_MUIBUILDER_H
#define _INLINE_MUIBUILDER_H

#ifndef CLIB_MUIBUILDER_PROTOS_H
#define CLIB_MUIBUILDER_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#ifndef MUIBUILDER_BASE_NAME
#define MUIBUILDER_BASE_NAME MUIBBase
#endif

#define MB_Open() \
	LP0(0x1e, BOOL, MB_Open, \
	, MUIBUILDER_BASE_NAME)

#define MB_Close() \
	LP0NR(0x24, MB_Close, \
	, MUIBUILDER_BASE_NAME)

#define MB_GetA(TagList) \
	LP1NR(0x2a, MB_GetA, struct TagItem *, TagList, a1, \
	, MUIBUILDER_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define MB_Get(tags...) \
	({ULONG _tags[] = {tags}; MB_GetA((struct TagItem *) _tags);})
#endif

#define MB_GetVarInfoA(varnb, TagList) \
	LP2NR(0x30, MB_GetVarInfoA, ULONG, varnb, d0, struct TagItem *, TagList, a1, \
	, MUIBUILDER_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define MB_GetVarInfo(varnb, tags...) \
	({ULONG _tags[] = {tags}; MB_GetVarInfoA((varnb), (struct TagItem *) _tags);})
#endif

#define MB_GetNextCode(type, code) \
	LP2NR(0x36, MB_GetNextCode, ULONG *, type, a0, char **, code, a1, \
	, MUIBUILDER_BASE_NAME)

#define MB_GetNextNotify(type, code) \
	LP2NR(0x3c, MB_GetNextNotify, ULONG *, type, a0, char **, code, a1, \
	, MUIBUILDER_BASE_NAME)

#endif /*  _INLINE_MUIBUILDER_H  */
