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

IPTR GenericOOP::DoMtd(Object* pObject, IPTR *pMsg)
{
   return Utility->CallHookPkt((const Hook*)OCLASS(pObject), pObject, pMsg);
}

IPTR GenericOOP::DoSuperMtd(IClass *pClass, Object* pObject, IPTR *pMsg)
{
   return Utility->CallHookPkt((Hook*)pClass->cl_Super, pObject, pMsg);
}

IPTR GenericOOP::NewObj(char* Name, IPTR FirstTag, ...)          // me hates mos for that.
{
   va_list ap;
   IPTR *params = new IPTR [128];
   IPTR pos = 0;
   va_start(ap, FirstTag);
      
   params[pos] = FirstTag;
   while (params[pos++] != 0)
   {
      params[pos++] = va_arg(ap, IPTR);
      params[pos] = va_arg(ap, IPTR);         
      ASSERT(pos < 128);
   }

   pos = (IPTR)Intuition->NewObjectA(0, Name, (struct TagItem*)params);
   delete [] params;
   return pos;
}

IPTR GenericOOP::NewObj(Class* cls, IPTR FirstTag, ...)          // me hates mos for that.
{
   va_list ap;
   IPTR *params = new IPTR [128];
   IPTR pos = 0;
   va_start(ap, FirstTag);
      
   params[pos] = FirstTag;
   while (params[pos++] != 0)
   {
      params[pos++] = va_arg(ap, IPTR);
      params[pos] = va_arg(ap, IPTR);         
      ASSERT(pos < 128);
   }

   pos = (IPTR)Intuition->NewObjectA((IClass*)cls, 0, (struct TagItem*)params);
   
   delete [] params;
   return pos;
}

void GenericOOP::DisposeObj(IPTR *obj)
{
   Intuition->DisposeObject(obj);
}

void GenericOOP::AddChildObj(Object *parent, IPTR *child)
{
   DoMtd(parent, ARRAY(OM_ADDMEMBER, (IPTR)child));
}

void GenericOOP::RemChildObj(Object *parent, IPTR *child)
{
   DoMtd(parent, ARRAY(OM_REMMEMBER, (IPTR)child));
}

GenericOOP::~GenericOOP()
{
}

GenericOOP::GenericOOP()
{
   ASSERTS(Utility != 0, "Please create library spool!");
}

