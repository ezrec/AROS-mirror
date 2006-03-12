/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id$
 *
 *****************************************************************************/

#ifdef __AROS__

#include <libraries/mui.h>
#include <aros/machine.h>

#define MGTK_HOOK_OBJECT_POINTER
#define MGTK_HOOK_APTR

#define HOOKPROTO(name, ret, obj, param) AROS_UFH2(ret, name, AROS_UFHA(APTR, param, A3), AROS_UFHA(Object *, obj, D0))

#define MakeHook(hookname, funcname) struct Hook hookname = {{NULL, NULL}, \
    (HOOKFUNC)funcname, NULL, NULL}

#define MGTK_USERFUNC_INIT AROS_USERFUNC_INIT
#define MGTK_USERFUNC_EXIT AROS_USERFUNC_EXIT

#endif
