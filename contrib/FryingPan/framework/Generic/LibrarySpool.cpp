/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "LibrarySpool.h"
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <libclass/dos.h>
#include <libclass/intuition.h>

using namespace GenNS;
extern struct Library* SysBase;

   LibrarySpool     *LibrarySpool::LibSpool = 0;
   UtilityIFace     *Utility = 0;
   ExecIFace        *Exec   = 0;
   DOSIFace         *DOS    = 0;
   IntuitionIFace   *Intuition = 0;


LibrarySpool::LibrarySpool()
{  
    Exec    = ExecIFace::GetInstance(SysBase);
    Utility = UtilityIFace::GetInstance(0);
    DOS     = DOSIFace::GetInstance(0);
    Intuition = IntuitionIFace::GetInstance(0);
}

LibrarySpool *LibrarySpool::Init()
{
   if (LibrarySpool::LibSpool == 0)
      LibrarySpool::LibSpool = new LibrarySpool();

   return LibrarySpool::LibSpool;
}

void LibrarySpool::Exit()
{
   LibrarySpool *tmp = LibrarySpool::LibSpool;

   LibrarySpool::LibSpool = 0;
   if (tmp != 0)
      delete tmp;
}

LibrarySpool::~LibrarySpool()
{
   Utility->FreeInstance();
   DOS->FreeInstance();
   Intuition->FreeInstance();
   Exec->FreeInstance();
}

