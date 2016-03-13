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

#define __NOLIBBASE__
#include <GenericLib/Library.h>
#include <Startup/Startup.h>
#include "Headers.h"
#include "Main.h"
#include "Config.h"

   LIBRARY("Optical", "$VER: Optical 2.0 (" __DATE__ " " __TIME__ ") Tomasz Wiszkowski", 2);

   GATE1(IPTR,    DoLibMethodA,                          IPTR,  a0);
   GATE0(class IOptItem*,  OptCreateDisc);
   LIB_FT_Begin
      LIB_FT_Function(DoLibMethodA)
      LIB_FT_Function(OptCreateDisc)
   LIB_FT_End

bool Lib_SetUp()
{
   if (false == __setup())
   {
      return false;
   }

   return SetUp();
}

void Lib_CleanUp()
{
   CleanUp();
   __cleanup();
}

bool Lib_Acquire()
{
   return true;
}

void Lib_Release()
{
   if (Cfg != 0)
      Cfg->onWrite();
}

