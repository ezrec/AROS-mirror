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

#ifndef _ENGINE_CALLS_H_
#define _ENGINE_CALLS_H_

#include <libclass/exec.h>
#include <GenericLib/Calls.h>
#include <Generic/LibrarySpool.h>

struct ScanData;

class IEngine;

   BEGINDECL(Engine, "fryingpan.engine")
      FUNC1(IEngine*,         getEngine,        5,    int,              which,   d0);
      FUNC1(const ScanData*,  scanDevice,       6,    const char*,      device,  a0);
      PROC1(                  freeScanData,     7,    const ScanData*,  data,    a0);
   ENDDECL

#endif
