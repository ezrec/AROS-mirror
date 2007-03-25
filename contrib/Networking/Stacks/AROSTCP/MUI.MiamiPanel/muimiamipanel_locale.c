/*
    Copyright © 1995-2004, The AROS Development Team. All rights reserved.
    $Id: locale.c 21058 2004-02-23 19:14:08Z chodorowski $
*/

#include <exec/types.h>
#include <proto/locale.h>

#define CATCOMP_ARRAY
#include "strings.h"

#define CATALOG_NAME     "System/MUI.miamipanel.catalog"
#define CATALOG_VERSION  0

#include <MUI/TheBar_mcc.h>
#include <libraries/gadtools.h>

/*** Variables **************************************************************/
struct Catalog *catalog;


/*** Functions **************************************************************/
/* Main *********************************************************************/
CONST_STRPTR _(ULONG id)
{
    if (LocaleBase != NULL && catalog != NULL)
    {
	return GetCatalogStr(catalog, id, CatCompArray[id].cca_Str);
    } 
    else 
    {
	return CatCompArray[id].cca_Str;
    }
}

void
localizeArray(UBYTE **strings,ULONG *ids)
{
    for (;;)
    {
        if (*ids) *strings++ = _(*ids++);
        else
        {
            *strings = NULL;
            break;
        }
    }
}

/***********************************************************************/

void
localizeMenus(struct NewMenu *menu,ULONG *ids)
{
    while (menu->nm_Type!=NM_END)
    {
        register ULONG id = *ids++;

        if (id && menu->nm_Label!=NM_BARLABEL)
            menu->nm_Label = _(id);

        menu++;
    }
}

/***********************************************************************/

void
localizeButtonsBar(struct MUIS_TheBar_Button *buttons,ULONG *ids)
{
    while (buttons->img!=MUIV_TheBar_End)
    {
        register ULONG t = *ids++;
        register ULONG h = *ids++;

        if (t) buttons->text = _(t);
        if (h) buttons->help = _(h);

        buttons++;
    }
}

/* Setup ********************************************************************/
/*VOID Locale_Initialize(VOID)
{
    if (LocaleBase != NULL)
    {
        catalog = OpenCatalog
        ( 
            NULL, CATALOG_NAME, OC_Version, CATALOG_VERSION, TAG_DONE 
        );
    }
    else
    {
        catalog = NULL;
    }
}

VOID Locale_Deinitialize(VOID)
{
    if(LocaleBase != NULL && catalog != NULL) CloseCatalog(catalog);
}*/

