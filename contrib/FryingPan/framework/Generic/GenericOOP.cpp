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

#include "GenericOOP.h"
#include "LibrarySpool.h"
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <libclass/intuition.h>
#include <stdarg.h>
#include "Debug.h"

using namespace GenNS;

uint32 GenericOOP::DoMtd(Object* pObject, uint32 *pMsg)
{
   return Utility->CallHookPkt((const Hook*)OCLASS(pObject), pObject, pMsg);
}

uint32 GenericOOP::DoSuperMtd(IClass *pClass, Object* pObject, uint32 *pMsg)
{
   return Utility->CallHookPkt((Hook*)pClass->cl_Super, pObject, pMsg);
}

uint32 *GenericOOP::NewObj(char* Name, uint32 FirstTag, ...)          // me hates mos for that.
{
   va_list ap;
   uint32 *params = new uint32 [128];
   int pos = 0;
   va_start(ap, FirstTag);
      
   params[pos] = FirstTag;
   while (params[pos++] != 0)
   {
      params[pos++] = va_arg(ap, unsigned long);
      params[pos] = va_arg(ap, unsigned long);         
      ASSERT(pos < 128);
   }

   pos = (int)Intuition->NewObjectA(0, Name, (struct TagItem*)params);
   delete [] params;
   return (uint32*)pos;
}

uint32 *GenericOOP::NewObj(Class* cls, uint32 FirstTag, ...)          // me hates mos for that.
{
   va_list ap;
   uint32 *params = new uint32 [128];
   int pos = 0;
   va_start(ap, FirstTag);
      
   params[pos] = FirstTag;
   while (params[pos++] != 0)
   {
      params[pos++] = va_arg(ap, unsigned long);
      params[pos] = va_arg(ap, unsigned long);         
      ASSERT(pos < 128);
   }

   pos = (int)Intuition->NewObjectA((IClass*)cls, 0, (struct TagItem*)params);
   
   delete [] params;
   return (uint32*)pos;
}

void GenericOOP::DisposeObj(uint32 *obj)
{
   Intuition->DisposeObject(obj);
}

void GenericOOP::AddChildObj(Object *parent, uint32 *child)
{
   DoMtd(parent, ARRAY(OM_ADDMEMBER, (uint32)child));
}

void GenericOOP::RemChildObj(Object *parent, uint32 *child)
{
   DoMtd(parent, ARRAY(OM_REMMEMBER, (uint32)child));
}

GenericOOP::~GenericOOP()
{
}

GenericOOP::GenericOOP()
{
   ASSERTS(Utility != 0, "Please create library spool!");
}

