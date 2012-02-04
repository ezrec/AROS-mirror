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

#include "Synchronizer.h"
#include <libclass/exec.h>
#include "LibrarySpool.h"

using namespace GenNS;

Synchronizer::Synchronizer()
{
   Exec->InitSemaphore(&hSemaphore);
}

Synchronizer::~Synchronizer()
{
}

bool Synchronizer::Attempt()
{
   if (Exec->AttemptSemaphore(&hSemaphore))
   {
      return true;
   }
   return false;      
}

void Synchronizer::Acquire()
{
   Exec->ObtainSemaphore(&hSemaphore);
}

void Synchronizer::Release()
{
   Exec->ReleaseSemaphore(&hSemaphore);
}
