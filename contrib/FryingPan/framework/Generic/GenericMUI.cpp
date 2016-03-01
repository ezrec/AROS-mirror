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

#include "GenericMUI.h"
#include "LibrarySpool.h"
#include <intuition/classes.h>
#include <stdarg.h>
#include <libclass/utility.h>
#include <libclass/intuition.h>
#include <intuition/classusr.h>
#include "Debug.h"

using namespace GenNS;

Object *GenericMUI::MUI_MakeObjectX(IPTR Name, ...)
{
   va_list ap;
   IPTR *params = new IPTR [128];
   IPTR pos = 0;
   va_start(ap, Name);
      
   while ((params[pos++] = va_arg(ap, IPTR)) != 0)
   {
      params[pos++] = va_arg(ap, IPTR);
      ASSERT(pos < 128);
   }
      
   pos = (IPTR)MUIMaster->MUI_MakeObjectA(Name, (IPTR *)params);
   delete [] params;
   return (Object*)pos;
}
   
Object *GenericMUI::MUI_NewObject(const char* Name, IPTR FirstTag, ...)          // me hates mos for that.
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
      
   pos = (IPTR)MUIMaster->MUI_NewObjectA(Name, (struct TagItem*)params);
   delete [] params;
   return (Object*)pos;
}
  
   
   

