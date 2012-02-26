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

#include <GenericLib/Library.h>
#include <LibC/LibC.h>
#include <Generic/LibrarySpool.h>
#include "ClISO.h"

   LIBRARY("isobuilder.module", "$VER: isobuilder.module 1.0 Tomasz Wiszkowski", 1);
   GATE0(ClISO*, create);

   LIB_FT_Begin
      LIB_FT_Function(create)
   LIB_FT_End

   LibrarySpool *pSpool;
   uint32 StartupFlags = 0;

bool Lib_SetUp()
{
   __setup();

   pSpool = LibrarySpool::Init();

   return true;
}

void Lib_CleanUp()
{
   LibrarySpool::Exit();
   __cleanup();
}

bool Lib_Acquire()
{
   return true;
}

void Lib_Release()
{
}

ClISO *create()
{
   return new ClISO();
}
// vim: ts=3 et
