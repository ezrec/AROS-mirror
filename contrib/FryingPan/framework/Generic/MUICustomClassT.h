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

#ifndef _GENERIC_MUICUSTOMCLASST_H_
#define _GENERIC_MUICUSTOMCLASST_H_

#include "MUICustomClass.h"

using namespace GenNS;

template <class T>
class MUICustomClassT : public MUICustomClass
{
public:
                              MUICustomClassT(const char *parent) :
      MUICustomClass(parent)
   {
   }

protected:
   virtual GenericBOOPSI        *createObject(IClass *cls)
   {
      return new T(cls);
   }
};

#endif

