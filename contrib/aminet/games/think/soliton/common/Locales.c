/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <proto/exec.h>
#include <proto/locale.h>

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#define CATCOMP_ARRAY
#include "Catalog.h"

struct Catalog *    catalog    = NULL;
struct LocaleBase * LocaleBase = NULL;

char * GetStr(int num)
{
  struct CatCompArrayType* cca = (struct CatCompArrayType*)CatCompArray;

  while(cca->cca_ID != num)
    cca++;
  if(catalog)
    return (char *) GetCatalogStr(catalog, num, cca->cca_Str);
  return (char *) cca->cca_Str;
}

void InitLocale(void)
{
  if((LocaleBase = (struct LocaleBase *) OpenLibrary("locale.library", 38)))
    catalog = OpenCatalogA((struct Locale *) NULL, (STRPTR) "Soliton.catalog", 0);
}

void ExitLocale(void)
{
  if(catalog)
    CloseCatalog(catalog);
  if(LocaleBase)
    CloseLibrary((struct Library *) LocaleBase);
}


