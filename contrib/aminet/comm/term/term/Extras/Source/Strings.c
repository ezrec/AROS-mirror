/*
**	Strings.c
**
**	Declaration of builtin language strings
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#define CATCOMP_ARRAY

#if defined(__STORM__) || defined(__GNUC__)
#define const
#endif

#ifndef TERM_STRINGS_H
#include "Strings.h"
#endif	/* TERM_STRINGS_H */

struct CatCompArrayType	*AppStrings	= &CatCompArray[0];
WORD			 NumAppStrings	= sizeof(CatCompArray) / sizeof(struct CatCompArrayType);
