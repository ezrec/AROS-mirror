/*
 *   VPDF
 * 
 *   Copyright © 2008 Christian Rosentreter <tokai@binaryriot.org>
 *   Copyright © 2012 Michal Zukowski, Michal Wozniak
 *   All rights reserved.
 *
 *   $Id$
 */

#include <proto/locale.h>

/*  must be included before locale.h 
 */
#define CATCOMP_ARRAY
#define NEW_CATCOMP_ARRAY_IDS
#include "VPDF_strings.h"

#include "locale.h"

extern struct Library *LocaleBase;
static APTR           _locale_catalog;

void locale_init(void)
{
	if (LocaleBase)
	{
		_locale_catalog = OpenCatalog(NULL, "VPDF.catalog", OC_BuiltInLanguage, (ULONG) "english", TAG_DONE);
	}
}

void locale_cleanup(void)
{
	if (LocaleBase) 
	{
		CloseCatalog(_locale_catalog); /* NULL is save here */
	}
}


STRPTR locale_getstr(ULONG array_id)
{
	const struct CatCompArrayType *t = CatCompArray+array_id;

	return LocaleBase ? GetCatalogStr(_locale_catalog, t->cca_ID, t->cca_Str) : t->cca_Str;
}
