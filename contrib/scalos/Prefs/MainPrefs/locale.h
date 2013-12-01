// locale.h
// $Date$
// $Revision$

#ifndef LOCALE_H
#define LOCALE_H

struct Scalos_Prefs_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

CONST_STRPTR      GetLocString(LONG lStrnum);

#define	Scalos_Prefs_NUMBERS
#include STR(SCALOSLOCALE)


#endif

