/*
**	Shutdown 2.0 package, LocaleSupport.c module
**
**	Copyright © 1992 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#define STRINGARRAY

#include "shutdown.h"

struct LocaleBase	*LocaleBase;
STATIC struct Catalog	*Catalog;

	/* LocaleOpen(STRPTR CatalogName,STRPTR BuiltIn):
	 *
	 *	Open string translation tables.
	 */

VOID __regargs
LocaleOpen(STRPTR CatalogName,STRPTR BuiltIn)
{
	if(LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",0))
	{
		if(!(Catalog = OpenCatalog(NULL,CatalogName,
			OC_BuiltInLanguage,	BuiltIn,
		TAG_DONE)))
		{
			CloseLibrary(LocaleBase);

			LocaleBase = NULL;
		}
	}
}

	/* LocaleClose():
	 *
	 *	Close the translation tables.
	 */

VOID
LocaleClose(VOID)
{
	if(Catalog)
		CloseCatalog(Catalog);

	if(LocaleBase)
		CloseLibrary(LocaleBase);
}

	/* GetString(LONG ID):
	 *
	 *	Obtain a string from the translation pool.
	 */

STRPTR __regargs
GetString(LONG ID)
{
	STRPTR Builtin = NULL;

	if(AppStrings[ID] . as_ID == ID)
		Builtin = AppStrings[ID] . as_Str;
	else
	{
		WORD i;

		for(i = 0 ; i < (sizeof(AppStrings) / sizeof(struct AppString)) ; i++)
		{
			if(AppStrings[i] . as_ID == ID)
			{
				Builtin = AppStrings[i] . as_Str;

				break;
			}
		}
	}

	if(LocaleBase)
	{
		STRPTR String = GetCatalogStr(Catalog,ID,Builtin);

		if(String[0])
			return(String);
		else
			return(Builtin);
	}
	else
		return(Builtin);
}
