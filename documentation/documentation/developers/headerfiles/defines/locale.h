#ifndef DEFINES_LOCALE_H
#define DEFINES_LOCALE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/locale/locale.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for locale
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __CloseCatalog_WB(__LocaleBase, __arg1) \
        AROS_LC1NR(void, CloseCatalog, \
                  AROS_LCA(struct Catalog *,(__arg1),A0), \
        struct LocaleBase *, (__LocaleBase), 6, Locale)

#define CloseCatalog(arg1) \
    __CloseCatalog_WB(LocaleBase, (arg1))

#define __CloseLocale_WB(__LocaleBase, __arg1) \
        AROS_LC1NR(void, CloseLocale, \
                  AROS_LCA(struct Locale *,(__arg1),A0), \
        struct LocaleBase *, (__LocaleBase), 7, Locale)

#define CloseLocale(arg1) \
    __CloseLocale_WB(LocaleBase, (arg1))

#define __ConvToLower_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ConvToLower, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 8, Locale)

#define ConvToLower(arg1, arg2) \
    __ConvToLower_WB(LocaleBase, (arg1), (arg2))

#define __ConvToUpper_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ConvToUpper, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 9, Locale)

#define ConvToUpper(arg1, arg2) \
    __ConvToUpper_WB(LocaleBase, (arg1), (arg2))

#define __FormatDate_WB(__LocaleBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, FormatDate, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(const struct DateStamp *,(__arg3),A2), \
                  AROS_LCA(const struct Hook *,(__arg4),A3), \
        struct LocaleBase *, (__LocaleBase), 10, Locale)

#define FormatDate(arg1, arg2, arg3, arg4) \
    __FormatDate_WB(LocaleBase, (arg1), (arg2), (arg3), (arg4))

#define __FormatString_WB(__LocaleBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(APTR, FormatString, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(CONST_APTR,(__arg3),A2), \
                  AROS_LCA(const struct Hook *,(__arg4),A3), \
        struct LocaleBase *, (__LocaleBase), 11, Locale)

#define FormatString(arg1, arg2, arg3, arg4) \
    __FormatString_WB(LocaleBase, (arg1), (arg2), (arg3), (arg4))

#define __GetCatalogStr_WB(__LocaleBase, __arg1, __arg2, __arg3) \
        AROS_LC3(CONST_STRPTR, GetCatalogStr, \
                  AROS_LCA(const struct Catalog *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(CONST_STRPTR,(__arg3),A1), \
        struct LocaleBase *, (__LocaleBase), 12, Locale)

#define GetCatalogStr(arg1, arg2, arg3) \
    __GetCatalogStr_WB(LocaleBase, (arg1), (arg2), (arg3))

#define __GetLocaleStr_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(CONST_STRPTR, GetLocaleStr, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 13, Locale)

#define GetLocaleStr(arg1, arg2) \
    __GetLocaleStr_WB(LocaleBase, (arg1), (arg2))

#define __IsAlNum_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsAlNum, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 14, Locale)

#define IsAlNum(arg1, arg2) \
    __IsAlNum_WB(LocaleBase, (arg1), (arg2))

#define __IsAlpha_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsAlpha, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 15, Locale)

#define IsAlpha(arg1, arg2) \
    __IsAlpha_WB(LocaleBase, (arg1), (arg2))

#define __IsCntrl_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsCntrl, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 16, Locale)

#define IsCntrl(arg1, arg2) \
    __IsCntrl_WB(LocaleBase, (arg1), (arg2))

#define __IsDigit_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsDigit, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 17, Locale)

#define IsDigit(arg1, arg2) \
    __IsDigit_WB(LocaleBase, (arg1), (arg2))

#define __IsGraph_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsGraph, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 18, Locale)

#define IsGraph(arg1, arg2) \
    __IsGraph_WB(LocaleBase, (arg1), (arg2))

#define __IsLower_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsLower, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 19, Locale)

#define IsLower(arg1, arg2) \
    __IsLower_WB(LocaleBase, (arg1), (arg2))

#define __IsPrint_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsPrint, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 20, Locale)

#define IsPrint(arg1, arg2) \
    __IsPrint_WB(LocaleBase, (arg1), (arg2))

#define __IsPunct_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsPunct, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 21, Locale)

#define IsPunct(arg1, arg2) \
    __IsPunct_WB(LocaleBase, (arg1), (arg2))

#define __IsSpace_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsSpace, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 22, Locale)

#define IsSpace(arg1, arg2) \
    __IsSpace_WB(LocaleBase, (arg1), (arg2))

#define __IsUpper_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsUpper, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 23, Locale)

#define IsUpper(arg1, arg2) \
    __IsUpper_WB(LocaleBase, (arg1), (arg2))

#define __IsXDigit_WB(__LocaleBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IsXDigit, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct LocaleBase *, (__LocaleBase), 24, Locale)

#define IsXDigit(arg1, arg2) \
    __IsXDigit_WB(LocaleBase, (arg1), (arg2))

#define __OpenCatalogA_WB(__LocaleBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct Catalog *, OpenCatalogA, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(const struct TagItem *,(__arg3),A2), \
        struct LocaleBase *, (__LocaleBase), 25, Locale)

#define OpenCatalogA(arg1, arg2, arg3) \
    __OpenCatalogA_WB(LocaleBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(LOCALE_NO_INLINE_STDARG)
#define OpenCatalog(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenCatalogA((arg1), (arg2), (const struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __OpenLocale_WB(__LocaleBase, __arg1) \
        AROS_LC1(struct Locale *, OpenLocale, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
        struct LocaleBase *, (__LocaleBase), 26, Locale)

#define OpenLocale(arg1) \
    __OpenLocale_WB(LocaleBase, (arg1))

#define __ParseDate_WB(__LocaleBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, ParseDate, \
                  AROS_LCA(struct Locale *,(__arg1),A0), \
                  AROS_LCA(struct DateStamp *,(__arg2),A1), \
                  AROS_LCA(CONST_STRPTR,(__arg3),A2), \
                  AROS_LCA(struct Hook *,(__arg4),A3), \
        struct LocaleBase *, (__LocaleBase), 27, Locale)

#define ParseDate(arg1, arg2, arg3, arg4) \
    __ParseDate_WB(LocaleBase, (arg1), (arg2), (arg3), (arg4))

#define __LocalePrefsUpdate_WB(__LocaleBase, __arg1) \
        AROS_LC1(struct Locale *, LocalePrefsUpdate, \
                  AROS_LCA(struct Locale *,(__arg1),A0), \
        struct LocaleBase *, (__LocaleBase), 28, Locale)

#define LocalePrefsUpdate(arg1) \
    __LocalePrefsUpdate_WB(LocaleBase, (arg1))

#define __StrConvert_WB(__LocaleBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(ULONG, StrConvert, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(APTR,(__arg3),A2), \
                  AROS_LCA(ULONG,(__arg4),D0), \
                  AROS_LCA(ULONG,(__arg5),D1), \
        struct LocaleBase *, (__LocaleBase), 29, Locale)

#define StrConvert(arg1, arg2, arg3, arg4, arg5) \
    __StrConvert_WB(LocaleBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __StrnCmp_WB(__LocaleBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(LONG, StrnCmp, \
                  AROS_LCA(const struct Locale *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(CONST_STRPTR,(__arg3),A2), \
                  AROS_LCA(LONG,(__arg4),D0), \
                  AROS_LCA(ULONG,(__arg5),D1), \
        struct LocaleBase *, (__LocaleBase), 30, Locale)

#define StrnCmp(arg1, arg2, arg3, arg4, arg5) \
    __StrnCmp_WB(LocaleBase, (arg1), (arg2), (arg3), (arg4), (arg5))

__END_DECLS

#endif /* DEFINES_LOCALE_H*/
