/***************************************************************************

 NListtree.mcc - New Listtree MUI Custom Class
 Copyright (C) 1999-2004 by Carsten Scholling <aphaso@aphaso.de>,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <libraries/mui.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <proto/muimaster.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#if defined(__AROS__)
 #include "private.h"
#elif defined(__PPC__)
 #pragma pack(2)
 #include "private.h"
 #pragma pack()
#else
 #include "private.h"
#endif

#include "NListtree.h"
#include "rev.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         MUIC_NListtree
#define SUPERCLASS    MUIC_NList

#define INSTDATA			NListtree_Data

#define UserLibID			"$VER: NListtree.mcc " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define USEDCLASSESP  used_classesP
#define USEDCLASSES   used_classes
static const STRPTR used_classesP[] = { "NListtree.mcp", "NListviews.mcp", NULL };
static const STRPTR used_classes[]  = { "NList.mcc", NULL };

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#include "mccheader.c"
