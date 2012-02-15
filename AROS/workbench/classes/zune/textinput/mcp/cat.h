/*
** $Id$
*/

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "textinput_cat.h"

#define GS(x) (catalog?(char*)GetCatalogStr(catalog,MSG_##x,MSG_##x##_STR):MSG_##x##_STR)
