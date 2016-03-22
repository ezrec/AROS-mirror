/*
 * localize.c
 * ==========
 * Handles localization of application.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <libraries/gadtools.h>
#include <proto/gadtools.h>
#include <intuition/intuition.h>
#include <libraries/locale.h>
#include <proto/locale.h>
#include <proto/alib.h>

#include "localize.h"

char * GetString(struct LocaleInfo * li,
                 LONG stringNum);
		 
struct LocaleBase *LocaleBase = NULL;

static struct LocaleInfo   li;

#if 0
STRPTR __asm
GetString (
   register __a0 struct LocaleInfo *li,
   register __d0 LONG stringNum);
#endif

extern struct Library * GadToolsBase;

void
init_locale (
   char   *catalog,
   ULONG   version)
{
   li.li_LocaleBase = NULL;
   if (NULL != (LocaleBase = (struct LocaleBase *)OpenLibrary ("locale.library", 38L)))
   {
      li.li_LocaleBase = LocaleBase;
      li.li_Catalog = OpenCatalog (NULL, catalog,
                                   OC_Version, version, TAG_DONE);
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
         nm[i].nm_CommKey = GetString (&li, (IPTR)nm[i].nm_Label);
         nm[i].nm_Label = nm[i].nm_CommKey + 2;
         if (nm[i].nm_CommKey[0] == ' ')
            nm[i].nm_CommKey = NULL;
      }
   }
   
   if (NULL != (menus = CreateMenusA (nm, (struct TagItem *)&tag)))
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
