/*
 *		LANGUAGE.C													vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		This module deals with localisation and sets up the string table
 *		used by all other parts of SnoopDos.
 */

#define CATCOMP_ARRAY

#define SNOOPDOS_CAT		"SnoopDos.catalog"

#include "system.h"
#include "snoopdos.h"

#ifdef __amigaos4__
struct Library      *LocaleBase;
struct LocaleIFace  *ILocale;
#else
struct LocaleBase	*LocaleBase;
#endif
struct Catalog		*SnoopDosCat;

/*
 *		InitTextTable()
 *
 *		Initialises the global text table from the default string array
 *		(created for us by CatComp in SNOOPTEXT.H)
 *
 *		We build our own table because CatComp generates a "packed" array
 *		which doesn't leave gaps for any strings we might have deleted
 *		from the language file during development. Our own array allows
 *		us to always directly obtain a string by saying MSG(MSG_FILEIO_GAD)
 *		or something similar.
 */
void InitTextTable(void)
{
	int i;

	for (i = 0; CatCompArray[i].cca_ID != NUM_OF_MSGS; i++)
		TextTable[CatCompArray[i].cca_ID] = CatCompArray[i].cca_Str;
}

/*
 *		InitLocale(catalogname)
 *
 *		Initialises the text table using the user's current Locale.
 *		'name' is the name of the catalog to use; this can be an
 *		absolute pathname to a different catalog if you like.
 *
 *		Note that you must have called InitTextTable() before calling
 *		this. Note also that you should call CleanupLocale() before
 *		exiting the program, even if the InitLocale() didn't work.
 */		
void InitLocale(char *langname)
{
	APTR oldwinptr = *TaskWindowPtr;
	int i;

	/*
	 *		We disable window requesters when looking for our catalog
	 *		since otherwise, if ENV: hasn't been assigned, we get requesters
	 *		asking for ENV: to be inserted which is a bit annoying if you
	 *		run SnoopDos after booting with no startup-sequence.
	 */
#ifdef __amigaos4__
	LocaleBase = OpenLib("locale.library", 0,(struct Interface **)(&ILocale));
#else
	LocaleBase	  = (struct LocaleBase *)OpenLibrary("locale.library", 0);
#endif
	if (!LocaleBase)
		return;
	
	*TaskWindowPtr = (APTR)-1;
	if (langname && *langname) {
		SnoopDosCat = OpenCatalog(NULL, SNOOPDOS_CAT,
										OC_BuiltInLanguage,	"english",
										OC_Language,		langname,
										TAG_DONE);
	} else {
		SnoopDosCat = OpenCatalog(NULL, SNOOPDOS_CAT,
										OC_BuiltInLanguage,	"english",
										TAG_DONE);
	}
	*TaskWindowPtr = oldwinptr;

	if (!SnoopDosCat) {
		/* Use default language */
		return;
	}
	
	for (i = 0; i < NUM_OF_MSGS; i++)
		TextTable[i] = GetCatalogStr(SnoopDosCat, i, TextTable[i]);
}

/*
 *		CleanupLocale()
 *
 *		Closes any resources opened by InitLocale()
 */
void CleanupLocale(void)
{
	if (SnoopDosCat)		CloseCatalog(SnoopDosCat),	SnoopDosCat = NULL;
#ifdef __amigaos4__
	if (ILocale)			DropInterface((struct Interface*)ILocale),		ILocale = NULL;
#endif
	if (LocaleBase)			CloseLibrary(LocaleBase),	LocaleBase	= NULL;
}
