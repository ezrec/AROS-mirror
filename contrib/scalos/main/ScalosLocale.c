// ScalosLocale.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/execbase.h>
#include <libraries/locale.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

void NewLocalePrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct SM_NewPreferences *smnp;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_LOCALEPREFS));

	smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
	if (smnp)
		{
		smnp->smnp_PrefsFlags = SMNPFLAGF_LOCALEPREFS;
		PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
		}
}


BOOL ChangedLocalePrefs(struct MainTask *mt)
{
///
	struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
	BOOL Changed = FALSE;

	CleanupLocale();

	Delay(3 * 50);		// Wait till locale.libary took notice of changed locale preferences

	InitLocale();

	// AppIcons will not get re-created like other icons,
	// so we do a ReLayout here
	DoMethod(iwtMain->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_ReLayoutIcons);

	return Changed;
///
}


void InitLocale(void)
{
///
	d1(kprintf("%s/%s/%ld: LocaleBase=%08lx\n", __FILE__, __FUNC__, __LINE__, LocaleBase));

	if (NULL == LocaleBase)
		return;

	ScalosLocale = OpenLocale(NULL);
	d1(kprintf("%s/%s/%ld: ScalosLocale=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosLocale));
	if (NULL == ScalosLocale)
		return;

	d1(kprintf("%s/%s/%ld: LocaleName=<%s>  Language=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, ScalosLocale->loc_LocaleName, ScalosLocale->loc_LanguageName));

	if (ScalosLocale->loc_GroupSeparator)
		CurrentPrefs.pref_GroupSeparator = *ScalosLocale->loc_GroupSeparator;

	ScalosCatalog = OpenCatalogA(ScalosLocale, (STRPTR) "Scalos/Scalos.catalog", NULL);

	d1(kprintf("%s/%s/%ld: ScalosCatalog=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosCatalog));
	if (ScalosCatalog)
		{
		d1(kprintf("%s/%s/%ld: cat_Version=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosCatalog->cat_Version));

		if (ScalosCatalog->cat_Version < 40)
			{
			CloseCatalog(ScalosCatalog);
			ScalosCatalog = NULL;
			}
		}
///
}


void CleanupLocale(void)
{
	if (LocaleBase)
		{
		if (ScalosCatalog)
			CloseCatalog(ScalosCatalog);
		if (ScalosLocale)
			CloseLocale(ScalosLocale);

		ScalosCatalog = NULL;
		ScalosLocale = NULL;
		}
}



