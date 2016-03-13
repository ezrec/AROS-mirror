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

#ifndef _GENERIC_MUICUSTOMCLASS_H_
#define _GENERIC_MUICUSTOMCLASS_H_

#include "GenericMUI.h"
#include "HookT.h"
#include <intuition/classusr.h>
#include "Registers.h"
#include "LibrarySpool.h"
#include <stdarg.h>
#include <libclass/intuition.h>
#include <libraries/mui.h>


   /*
    * HOW TO USE IT?
    * it is best to create the object that derives from the GenericBOOPSI class
    * simply replace DoMtd() with your own method to get the desired effect
    */

namespace GenNS
{
   class MUICustomClass : public GenericMUI
   {
   protected:
      virtual GenericBOOPSI     *createObject(IClass *cls)    = 0;

   private:
      void                      *getDispatcher();
      virtual IPTR               dispatch(IClass *cls, Object* obj, IPTR msg);

   protected:
      MUI_CustomClass           *pMUIClass;
      IClass                    *pClass;

   public:
      MUICustomClass(const char *parent);
      virtual                   ~MUICustomClass();
      Object                    *Create(IPTR lTag1, ...);

   private:
#if defined (__AROS__)
      AROS_UFP4(static IPTR, FDispatchCaller,
         AROS_UFPA(struct IClass *, pClass, A0),
         AROS_UFPA(Object *, pObject, A2),
         AROS_UFPA(IPTR, pMessage, A1),
         AROS_UFPA(APTR, data, A6));
#elif defined (__AMIGAOS4__)
      static unsigned long       FDispatchCaller(IClass *pClass, Object* pObject, unsigned long *pMessage);
#elif defined (__MORPHOS__)
      static unsigned long       FDispatchCallerFunc();
      static const unsigned long FDispatchCaller[2];
#elif defined (__mc68000)
      static unsigned long       FDispatchCaller();
#endif
   };
};

#endif

