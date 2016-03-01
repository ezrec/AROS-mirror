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

#include "MUICustomClass.h"
#undef MUI_CreateCustomClass
#undef MUI_DeleteCustomClass
#include <libclass/intuition.h>
#include "Debug.h"

using namespace GenNS;

#undef NewObjectA

void *MUICustomClass::getDispatcher()
{
   return (void*)&FDispatchCaller;
}

uint MUICustomClass::dispatch(IClass *cls, Object* obj, unsigned long *msg)
{
   GenericOOP       *co;
   GenericOOP      **ptr;
   unsigned long     ret = 0;

   /*
    * man, this was tricky.
    * turns out that there is a big chance someone will call methods on emerging object.
    * that is obvious to a point. if memory is not set up properly to zeroes 
    * we will have a nasty flight..
    *
    * you dont wanna dig too deep here...
    */

   switch (*msg)
   {
      case OM_NEW:
         co    = createObject(cls);
         ret   = 0;
         ASSERT (co  != NULL);
         if (co != NULL)
         {
            obj = (Object*)co->DoMtd(obj, msg);
            ASSERT (obj != NULL);

            if (NULL != obj)
            {
               ptr   = (GenericOOP**)INST_DATA(cls, obj);
               ASSERT(NULL != ptr);
               if (NULL != ptr)
                  *ptr  = co;
               ret = (unsigned long)obj;
            }
         }
         break;
            
      case OM_DISPOSE:
         ptr   = (GenericOOP**)INST_DATA(cls, obj);
         co    = *ptr;
         if (NULL != co)
         {
            ret   = co->DoMtd(obj, msg);
            delete co;
         }
         else
         {
            ret   = DoSuperMtd(cls, obj, msg);
         }
         break;
            
      default:
         ptr   = (GenericOOP**)INST_DATA(cls, obj);
         co    = *ptr;
         if (NULL != co)
         {
            ret   = co->DoMtd(obj, msg);
         }
         else
         {
            ret   = DoSuperMtd(cls, obj, msg);
         }
         break;
   }  

   return ret;
}

MUICustomClass::MUICustomClass(const char *parent)
{
   pMUIClass = 0;
   pClass    = 0;

   pMUIClass = MUIMaster->MUI_CreateCustomClass(0, const_cast<char*>(parent), 0, sizeof(GenericOOP*), getDispatcher());

   ASSERTS(pMUIClass != 0, "Unable to create MUI Custom Class");
   if (pMUIClass != NULL)
   {
      pClass = pMUIClass->mcc_Class;
      pClass->cl_UserData = (IPTR)this;
   }
}

MUICustomClass::~MUICustomClass()
{
   if (NULL != pMUIClass)
      MUIMaster->MUI_DeleteCustomClass(pMUIClass);
}
   
Object *MUICustomClass::Create(IPTR lTag1, ...)
{
   va_list        ap;
   IPTR *params = new IPTR [128];
   IPTR           pos = 0;
   
   va_start(ap, lTag1);
   
   params[pos] = lTag1;

   while (params[pos++] != 0)
   {
      params[pos++]    = va_arg(ap, IPTR);      // data
      params[pos  ]    = va_arg(ap, IPTR);      // tag
      ASSERT(pos < 128);
   }

   pos = (IPTR)Intuition->NewObjectA(pClass, 0, (struct TagItem*)params);

   delete [] params;
   return (Object*)pos;
};




#ifdef __AROS__
AROS_UFH4(IPTR, GenNS::MUICustomClass::FDispatchCaller,
   AROS_UFHA(struct IClass *, pClass, A0),
   AROS_UFHA(Object *, pObject, A2),
   AROS_UFHA(unsigned long *, pMessage, A1),
   AROS_UFHA(APTR, data, A6))
{
   AROS_USERFUNC_INIT

   return ((MUICustomClass*)pClass->cl_UserData)->dispatch(pClass, pObject,
      pMessage);

   AROS_USERFUNC_EXIT
}
    
#elif defined (__AMIGAOS4__)

unsigned long MUICustomClass::FDispatchCaller(IClass *pClass, Object* pObject, unsigned long *pMessage)
{    
   return ((MUICustomClass*)pClass->cl_UserData)->dispatch(pClass, pObject, pMessage);
}

#elif defined (__MORPHOS__)

unsigned long MUICustomClass::FDispatchCallerFunc()
{
   IClass        *pClass   = (IClass*)REG_A0;
   Object        *pObject  = (Object*)REG_A2;
   unsigned long *pMessage = (unsigned long*)REG_A1;

   return ((MUICustomClass*)pClass->cl_UserData)->dispatch(pClass, pObject, pMessage);
}
   
const unsigned long MUICustomClass::FDispatchCaller[2] = 
{
   0xff000000, 
   (unsigned long)&FDispatchCallerFunc 
};

#elif defined (__mc68000)

unsigned long MUICustomClass::FDispatchCaller()
{
   IClass        *pClass;
   Object        *pObject;
   unsigned long *pMessage;

   asm volatile ("mov.l a0,%0\n\t"
                 "mov.l a1,%1\n\t"
                 "mov.l a2,%2\n\t"
                 : "=omg"(pClass),
                   "=omg"(pMessage),
                   "=omg"(pObject)
                 : 
                 : "a0", "a1", "a2", "cc", "memory");

  
   return ((MUICustomClass*)pClass->cl_UserData)->dispatch(pClass, pObject, pMessage);
}

#endif






