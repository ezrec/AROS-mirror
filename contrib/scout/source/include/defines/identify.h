/* Automatically generated header! Do not edit! */

#ifndef _INLINE_IDENTIFY_H
#define _INLINE_IDENTIFY_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef IDENTIFY_BASE_NAME
#define IDENTIFY_BASE_NAME IdentifyBase
#endif /* !IDENTIFY_BASE_NAME */

#define IdAlert(___ID, ___TagList) \
	AROS_LC2(LONG, IdAlert, \
	AROS_LCA(ULONG, (___ID), D0), \
	AROS_LCA(struct TagItem *, (___TagList), A0), \
	struct Library *, IDENTIFY_BASE_NAME, 7, /* s */)

#ifndef NO_INLINE_STDARG
#define IdAlertTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdAlert((a0), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdEstimateFormatSize(___String, ___Tags) \
	AROS_LC2(ULONG, IdEstimateFormatSize, \
	AROS_LCA(STRPTR, (___String), A0), \
	AROS_LCA(struct TagItem *, (___Tags), A1), \
	struct Library *, IDENTIFY_BASE_NAME, 12, /* s */)

#ifndef NO_INLINE_STDARG
#define IdEstimateFormatSizeTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdEstimateFormatSize((a0), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdExpansion(___TagList) \
	AROS_LC1(LONG, IdExpansion, \
	AROS_LCA(struct TagItem *, (___TagList), A0), \
	struct Library *, IDENTIFY_BASE_NAME, 5, /* s */)

#ifndef NO_INLINE_STDARG
#define IdExpansionTags(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdExpansion((struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdFormatString(___String, ___Buffer, ___Length, ___Tags) \
	AROS_LC4(ULONG, IdFormatString, \
	AROS_LCA(STRPTR, (___String), A0), \
	AROS_LCA(STRPTR, (___Buffer), A1), \
	AROS_LCA(ULONG, (___Length), D0), \
	AROS_LCA(struct TagItem *, (___Tags), A2), \
	struct Library *, IDENTIFY_BASE_NAME, 11, /* s */)

#ifndef NO_INLINE_STDARG
#define IdFormatStringTags(a0, a1, a2, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdFormatString((a0), (a1), (a2), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdFunction(___LibName, ___Offset, ___TagList) \
	AROS_LC3(LONG, IdFunction, \
	AROS_LCA(STRPTR, (___LibName), A0), \
	AROS_LCA(LONG, (___Offset), D0), \
	AROS_LCA(struct TagItem *, (___TagList), A1), \
	struct Library *, IDENTIFY_BASE_NAME, 8, /* s */)

#ifndef NO_INLINE_STDARG
#define IdFunctionTags(a0, a1, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdFunction((a0), (a1), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdHardware(___Type, ___TagList) \
	AROS_LC2(STRPTR, IdHardware, \
	AROS_LCA(ULONG, (___Type), D0), \
	AROS_LCA(struct TagItem *, (___TagList), A0), \
	struct Library *, IDENTIFY_BASE_NAME, 6, /* s */)

#ifndef NO_INLINE_STDARG
#define IdHardwareTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdHardware((a0), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdHardwareNum(___Type, ___TagList) \
	AROS_LC2(IPTR, IdHardwareNum, \
	AROS_LCA(ULONG, (___Type), D0), \
	AROS_LCA(struct TagItem *, (___TagList), A0), \
	struct Library *, IDENTIFY_BASE_NAME, 9, /* s */)

#ifndef NO_INLINE_STDARG
#define IdHardwareNumTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; IdHardwareNum((a0), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define IdHardwareUpdate() \
	AROS_LC0(void, IdHardwareUpdate, \
	struct Library *, IDENTIFY_BASE_NAME, 10, /* s */)

#endif /* !_INLINE_IDENTIFY_H */
