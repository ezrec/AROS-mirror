/****************************************************************************
*                                                                           *
* aros.h -- Lunapaint,                                                      *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#ifndef AROS_AROS_H
#define AROS_AROS_H

#if defined(__MORPHOS__)

#define AROS_UFHA(a, b, c) a b = ( a ) REG_ ## c
#define AROS_USERFUNC_INIT
#define AROS_USERFUNC_EXIT }

#define AROS_UFH3(a, b, c, d, e) \
a b ##PPC(void); \
static struct EmulLibEntry b = { TRAP_LIB, 0, (APTR) & b ##PPC }; \
a b ##PPC(void) { c ; d ; e ;

#define BOOPSI_DISPATCHER(a, b, c, d, e) \
a b ##PPC(void); \
static struct EmulLibEntry b = { TRAP_LIB, 0, (APTR) & b ##PPC }; \
a b ##PPC(void) { struct IClass * c = (APTR)REG_A0; APTR d = (APTR)REG_A2; Msg e = (APTR)REG_A1;

#define BOOPSI_DISPATCHER_END }

#include <exec/types.h>
ULONG XGET(APTR, ULONG);
APTR ImageButton(CONST_STRPTR text, CONST_STRPTR image);

#endif

#endif /* AROS_AROS_H */
