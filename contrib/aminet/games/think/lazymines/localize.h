/*
 * localize.h
 * ==========
 * Functions for localization.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef LOCALIZE_H
#define LOCALIZE_H

#include <exec/types.h>
#include <libraries/gadtools.h>

#define CATCOMP_NUMBERS
#include "strings.h"


void
init_locale (
   char   *catalog,
   ULONG   version);

void
finalize_locale (void);

char *
localized_string (
   LONG   string_num);

struct Menu *
CreateLocMenus (
   struct NewMenu  *new_menus,
   APTR             vis_info,
   ULONG            tag,
   ...);

#endif /* LOCALIZE_H */
