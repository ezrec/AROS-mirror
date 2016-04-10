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

#ifndef INCLUDE_H
#define INCLUDE_H 1

/*------------------------------------------------------------------------*/

#define NULL ((void *)0l)

/*------------------------------------------------------------------------*/

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <exec/exec.h>
#include <exec/execbase.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NFloattext_mcc.h>

#if !defined(__MORPHOS__) && !defined(__AROS__)
	#include "os3.h"
#endif

#include <proto/alib.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <string.h>
#include <stdarg.h>

/*------------------------------------------------------------------------*/

#ifdef __MORPHOS__
	#pragma pack(2)
#endif

#include "compiler.h"
#include "define.h"
#include "tags.h"
#include "rev.h"
#include "structures.h"

#include "app.h"
#include "listtree.h"
#include "main.h"
#include "misc.h"
#include "startup.h"
#include "string.h"

#ifdef __MORPHOS__
	#pragma pack()
#endif

/*------------------------------------------------------------------------*/

#endif /* INCLUDE_H */
