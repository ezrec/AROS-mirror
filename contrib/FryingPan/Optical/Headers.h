/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __Optical_Headers
#define __Optical_Headers

#define __NOLIBBASE__

#include <Generic/Generic.h>

#include <libclass/exec.h>
#include <libclass/dos.h>

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/ports.h>

#include <dos/dos.h>
#include <dos/dostags.h>

#include <utility/tagitem.h>
#include <devices/scsidisk.h>
#include <LibC/LibC.h>
   
   
   extern bool  Clear1, Clear2, AllClear;
   extern class Config        *Cfg;
   
#include "Optical.h"
#include "Internals.h"

#endif
