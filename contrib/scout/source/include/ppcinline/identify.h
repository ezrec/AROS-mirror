/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_IDENTIFY_H
#define _PPCINLINE_IDENTIFY_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef IDENTIFY_BASE_NAME
#define IDENTIFY_BASE_NAME IdentifyBase
#endif /* !IDENTIFY_BASE_NAME */

#define IdExpansion(__p0) \
	LP1(30, LONG , IdExpansion, \
		struct TagItem *, __p0, a0, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdFunction(__p0, __p1, __p2) \
	LP3(48, LONG , IdFunction, \
		STRPTR , __p0, a0, \
		LONG , __p1, d0, \
		struct TagItem *, __p2, a1, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdAlert(__p0, __p1) \
	LP2(42, LONG , IdAlert, \
		ULONG , __p0, d0, \
		struct TagItem *, __p1, a0, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdHardwareNum(__p0, __p1) \
	LP2(54, ULONG , IdHardwareNum, \
		ULONG , __p0, d0, \
		struct TagItem *, __p1, a0, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdHardware(__p0, __p1) \
	LP2(36, STRPTR , IdHardware, \
		ULONG , __p0, d0, \
		struct TagItem *, __p1, a0, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdEstimateFormatSize(__p0, __p1) \
	LP2(72, ULONG , IdEstimateFormatSize, \
		STRPTR , __p0, a0, \
		struct TagItem *, __p1, a1, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdFormatString(__p0, __p1, __p2, __p3) \
	LP4(66, ULONG , IdFormatString, \
		STRPTR , __p0, a0, \
		STRPTR , __p1, a1, \
		ULONG , __p2, d0, \
		struct TagItem *, __p3, a2, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IdHardwareUpdate() \
	LP0NR(60, IdHardwareUpdate, \
		, IDENTIFY_BASE_NAME, 0, 0, 0, 0, 0, 0)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)

#include <stdarg.h>

#define IdExpansionTags(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdExpansion((struct TagItem *)_tags);})

#define IdFunctionTags(__p0, __p1, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdFunction(__p0, __p1, (struct TagItem *)_tags);})

#define IdAlertTags(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdAlert(__p0, (struct TagItem *)_tags);})

#define IdHardwareNumTags(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdHardwareNum(__p0, (struct TagItem *)_tags);})

#define IdHardwareTags(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdHardware(__p0, (struct TagItem *)_tags);})

#define IdEstimateFormatSizeTags(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdEstimateFormatSize(__p0, (struct TagItem *)_tags);})

#define IdFormatStringTags(__p0, __p1, __p2, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	IdFormatString(__p0, __p1, __p2, (struct TagItem *)_tags);})

#endif

#endif /* !_PPCINLINE_IDENTIFY_H */
