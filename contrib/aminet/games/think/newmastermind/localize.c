/*
 * localize.c
 * ==========
 * Handles localization of application.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/locale.h>
#include "localize.h"

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/locale.h>


struct LocaleBase   *LocaleBase = NULL;

static struct LocaleInfo   li;


STRPTR /* __asm */
GetString (
   /*register __a0 */ struct LocaleInfo *li,
   /*register __d0 */ LONG stringNum);


void
init_locale (
   char  *catalog)
{
   li.li_LocaleBase = NULL;
   if ((LocaleBase = (struct LocaleBase *)OpenLibrary ("locale.library", 38L)))
   {
      li.li_LocaleBase = LocaleBase;
      li.li_Catalog = OpenCatalogA (NULL, catalog, NULL);
   }
}

void
finalize_locale (void)
{
   if (LocaleBase)
   {
      CloseCatalog (li.li_Catalog);
      CloseLibrary ((struct Library *)LocaleBase);
   }
}

char *
localized_string (
   LONG   string_num)
{
   return (char *)GetString (&li, string_num);
}

struct Menu *
CreateLocMenus (
   struct NewMenu  *new_menus,
   APTR             vis_info,
   ULONG            tag,
   ...)
{
   UWORD   i;
   struct NewMenu  *nm;
   struct Menu     *menus;
   
   i = 0;
   while (new_menus[i++].nm_Type != NM_END)
      ;
   
   if (!(nm = AllocVec (i * sizeof (struct NewMenu),
                        MEMF_CLEAR | MEMF_PUBLIC)))
   {
      return NULL;
   }
   
   while (i--)
   {
      nm[i] = new_menus[i];
      
      if (nm[i].nm_Label != NM_BARLABEL)
      {
         nm[i].nm_CommKey = GetString (&li, (LONG)nm[i].nm_Label);
         nm[i].nm_Label = nm[i].nm_CommKey + 2;
         if (nm[i].nm_CommKey[0] == ' ')
            nm[i].nm_CommKey = NULL;
      }
   }
   
   if ((menus = CreateMenusA (nm, (struct TagItem *)&tag)))
   {
      if (!(LayoutMenus (menus, vis_info,
                         GTMN_NewLookMenus, TRUE,
                         TAG_DONE)))
      {
         FreeMenus (menus);
         menus = NULL;
      }
   }
   
   FreeVec (nm);
   
   return menus;
}
