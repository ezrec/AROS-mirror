#ifndef CLIB_LOCALE_PROTOS_H
#define CLIB_LOCALE_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/locale/locale.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <utility/hooks.h>
#include <dos/datetime.h>
#include <libraries/locale.h>

__BEGIN_DECLS

struct Catalog *OpenCatalog
(
    const struct Locale  *locale,
    CONST_STRPTR name,
    Tag             tag1,
    ...
);

__END_DECLS


__BEGIN_DECLS

AROS_LP1(void, CloseCatalog,
         AROS_LPA(struct Catalog *, catalog, A0),
         LIBBASETYPEPTR, LocaleBase, 6, Locale
);
AROS_LP1(void, CloseLocale,
         AROS_LPA(struct Locale *, locale, A0),
         LIBBASETYPEPTR, LocaleBase, 7, Locale
);
AROS_LP2(ULONG, ConvToLower,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 8, Locale
);
AROS_LP2(ULONG, ConvToUpper,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 9, Locale
);
AROS_LP4(void, FormatDate,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(CONST_STRPTR, formatString, A1),
         AROS_LPA(const struct DateStamp *, date, A2),
         AROS_LPA(const struct Hook *, hook, A3),
         LIBBASETYPEPTR, LocaleBase, 10, Locale
);
AROS_LP4(APTR, FormatString,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(CONST_STRPTR, fmtTemplate, A1),
         AROS_LPA(CONST_APTR, dataStream, A2),
         AROS_LPA(const struct Hook *, putCharFunc, A3),
         LIBBASETYPEPTR, LocaleBase, 11, Locale
);
AROS_LP3(CONST_STRPTR, GetCatalogStr,
         AROS_LPA(const struct Catalog *, catalog, A0),
         AROS_LPA(ULONG, stringNum, D0),
         AROS_LPA(CONST_STRPTR, defaultString, A1),
         LIBBASETYPEPTR, LocaleBase, 12, Locale
);
AROS_LP2(CONST_STRPTR, GetLocaleStr,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, stringNum, D0),
         LIBBASETYPEPTR, LocaleBase, 13, Locale
);
AROS_LP2(ULONG, IsAlNum,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 14, Locale
);
AROS_LP2(ULONG, IsAlpha,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 15, Locale
);
AROS_LP2(ULONG, IsCntrl,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 16, Locale
);
AROS_LP2(ULONG, IsDigit,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 17, Locale
);
AROS_LP2(ULONG, IsGraph,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 18, Locale
);
AROS_LP2(ULONG, IsLower,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 19, Locale
);
AROS_LP2(ULONG, IsPrint,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 20, Locale
);
AROS_LP2(ULONG, IsPunct,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 21, Locale
);
AROS_LP2(ULONG, IsSpace,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 22, Locale
);
AROS_LP2(ULONG, IsUpper,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 23, Locale
);
AROS_LP2(ULONG, IsXDigit,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(ULONG, character, D0),
         LIBBASETYPEPTR, LocaleBase, 24, Locale
);
AROS_LP3(struct Catalog *, OpenCatalogA,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(CONST_STRPTR, name, A1),
         AROS_LPA(const struct TagItem *, tags, A2),
         LIBBASETYPEPTR, LocaleBase, 25, Locale
);
AROS_LP1(struct Locale *, OpenLocale,
         AROS_LPA(CONST_STRPTR, name, A0),
         LIBBASETYPEPTR, LocaleBase, 26, Locale
);
AROS_LP4(BOOL, ParseDate,
         AROS_LPA(struct Locale *, locale, A0),
         AROS_LPA(struct DateStamp *, date, A1),
         AROS_LPA(CONST_STRPTR, fmtTemplate, A2),
         AROS_LPA(struct Hook *, getCharFunc, A3),
         LIBBASETYPEPTR, LocaleBase, 27, Locale
);
AROS_LP1(struct Locale *, LocalePrefsUpdate,
         AROS_LPA(struct Locale *, locale, A0),
         LIBBASETYPEPTR, LocaleBase, 28, Locale
);
AROS_LP5(ULONG, StrConvert,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(CONST_STRPTR, string, A1),
         AROS_LPA(APTR, buffer, A2),
         AROS_LPA(ULONG, bufferSize, D0),
         AROS_LPA(ULONG, type, D1),
         LIBBASETYPEPTR, LocaleBase, 29, Locale
);
AROS_LP5(LONG, StrnCmp,
         AROS_LPA(const struct Locale *, locale, A0),
         AROS_LPA(CONST_STRPTR, string1, A1),
         AROS_LPA(CONST_STRPTR, string2, A2),
         AROS_LPA(LONG, length, D0),
         AROS_LPA(ULONG, type, D1),
         LIBBASETYPEPTR, LocaleBase, 30, Locale
);

__END_DECLS

#endif /* CLIB_LOCALE_PROTOS_H */
