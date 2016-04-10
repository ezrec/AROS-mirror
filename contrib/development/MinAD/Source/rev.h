/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef REV_H
#define REV_H 1

/*------------------------------------------------------------------------*/

#define APP_PROGNAME		"MinAD"
#define APP_DESCRIPTION	"MINimalistic AutoDoc"

#define APP_VER			"1"
#define APP_REV			"7"

#define APP_VERREV		APP_VER"."APP_REV

#ifdef __MORPHOS__
#define APP_BUILD			"ppc-morphos"
#elif __AROS__
#define APP_BUILD			"AROS"
#else
#define APP_BUILD			"m68k-amigaos"
#endif

/*------------------------------------------------------------------------*/

#define APP_AUTHOR		"Rupert Hausberger"

#define APP_COPYRIGHT	"©2005-"__YEAR__" "APP_AUTHOR
#define APP_HOMEPAGE 	"http://natmeg.mooo.com"

#define APP_LICENCE		"GPL"
#define APP_RIGHTS		"All Rights Reserved"

/*------------------------------------------------------------------------*/

#define APP_DATE			__AMIGADATE__

#define APP_VERSTAG		"\0$VER: "APP_PROGNAME" "APP_VERREV" ["APP_BUILD"] ("APP_DATE") "APP_COPYRIGHT

/*------------------------------------------------------------------------*/

#define APP_DISKOBJECT	"PROGDIR:"APP_PROGNAME
#define APP_HELPFILE		"PROGDIR:Docs/"APP_PROGNAME".guide"

#ifdef __MORPHOS__
#define APP_LOGOFILE		"PROGDIR:"APP_PROGNAME".info"
#else
#define APP_LOGOFILE		"PROGDIR:"APP_PROGNAME".logo"
#endif

/*------------------------------------------------------------------------*/

#endif /* REV_H */
