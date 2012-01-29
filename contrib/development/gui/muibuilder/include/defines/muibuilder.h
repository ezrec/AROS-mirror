/* Automatically generated header! Do not edit! */

#ifndef _INLINE_MUIBUILDER_H
#define _INLINE_MUIBUILDER_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef MUIBUILDER_BASE_NAME
#define MUIBUILDER_BASE_NAME MUIBBase
#endif /* !MUIBUILDER_BASE_NAME */

#define MB_Open() __MB_Open_WB(MUIBUILDER_BASE_NAME)
#define __MB_Open_WB(___base) \
	AROS_LC0(BOOL, MB_Open, \
	struct Library *, (___base), 5, Muibuilder)

#define MB_Close() __MB_Close_WB(MUIBUILDER_BASE_NAME)
#define __MB_Close_WB(___base) \
	AROS_LC0NR(void, MB_Close, \
	struct Library *, (___base), 6, Muibuilder)

#define MB_GetA(___TagList) __MB_GetA_WB(MUIBUILDER_BASE_NAME, ___TagList)
#define __MB_GetA_WB(___base, ___TagList) \
	AROS_LC1NR(void, MB_GetA, \
	AROS_LCA(struct TagItem *, (___TagList), A1), \
	struct Library *, (___base), 7, Muibuilder)

#ifndef NO_INLINE_STDARG
#define MB_Get(___tag1, ...) __MB_Get_WB(MUIBUILDER_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __MB_Get_WB(___base, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __MB_GetA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define MB_GetVarInfoA(___varnb, ___TagList) __MB_GetVarInfoA_WB(MUIBUILDER_BASE_NAME, ___varnb, ___TagList)
#define __MB_GetVarInfoA_WB(___base, ___varnb, ___TagList) \
	AROS_LC2NR(void, MB_GetVarInfoA, \
	AROS_LCA(ULONG, (___varnb), D0), \
	AROS_LCA(struct TagItem *, (___TagList), A1), \
	struct Library *, (___base), 8, Muibuilder)

#ifndef NO_INLINE_STDARG
#define MB_GetVarInfo(___varnb, ___tag1, ...) __MB_GetVarInfo_WB(MUIBUILDER_BASE_NAME, ___varnb, ___tag1, ## __VA_ARGS__)
#define __MB_GetVarInfo_WB(___base, ___varnb, ___tag1, ...) \
	({IPTR _tags[] = { (IPTR) ___tag1, ## __VA_ARGS__ }; __MB_GetVarInfoA_WB((___base), (___varnb), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define MB_GetNextCode(___type, ___code) __MB_GetNextCode_WB(MUIBUILDER_BASE_NAME, ___type, ___code)
#define __MB_GetNextCode_WB(___base, ___type, ___code) \
	AROS_LC2NR(void, MB_GetNextCode, \
	AROS_LCA(ULONG *, (___type), A0), \
	AROS_LCA(char **, (___code), A1), \
	struct Library *, (___base), 9, Muibuilder)

#define MB_GetNextNotify(___type, ___code) __MB_GetNextNotify_WB(MUIBUILDER_BASE_NAME, ___type, ___code)
#define __MB_GetNextNotify_WB(___base, ___type, ___code) \
	AROS_LC2NR(void, MB_GetNextNotify, \
	AROS_LCA(ULONG *, (___type), A0), \
	AROS_LCA(char **, (___code), A1), \
	struct Library *, (___base), 10, Muibuilder)

#endif /* !_INLINE_MUIBUILDER_H */
