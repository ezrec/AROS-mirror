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

#include "IHook.h"
#include <utility/hooks.h>

#ifdef __MORPHOS__
   #include <emul/emulregs.h>
#endif

using namespace GenNS;

#if defined (__AROS__) || defined (__AMIGAOS4__)
IPTR IHook::subCaller(Hook *pHook, IPTR pObject, IPTR pMessage)
{
   return ((IHook*)pHook->h_Data)->Call(pObject, pMessage);
}
#elif defined (__MORPHOS__)
uint32 IHook::subCaller()
{
   Hook*pHook    = (Hook*)REG_A0;
   uint32 pObject  = (uint32)REG_A2;
   uint32 pMessage = (uint32)REG_A1;
   return ((IHook*)pHook->h_Data)->Call(pObject, pMessage);
}
#elif defined (__mc68000)
uint32 IHook::subCaller()
{
   Hook    *pHook;
   uint32   pObject;
   uint32   pMessage;

   asm volatile( "mov.l a0,%0\n\t"
         "mov.l a1,%1\n\t"
         "mov.l a2,%2\n\t"
         : "=omg"(pHook), "=omg"(pMessage), "=omg"(pObject)
         :
         : "a0", "a1", "a2", "cc", "memory");

   return ((IHook*)pHook->h_Data)->Call(pObject, pMessage);
}
#endif

void IHook::Initialize()
{
   hHook.h_Data      =  (void*)(this);
#ifdef __MORPHOS__
   hMOSCall[0]       =  0xff000000;
   hMOSCall[1]       =  (unsigned long)&subCaller;
   hHook.h_Entry     =  (unsigned long(*)())&hMOSCall;
#else
   hHook.h_Entry     =  (APTR)&subCaller;
#endif
   hHook.h_SubEntry  =  0;

};                        

IHook::IHook()
{
   Initialize();
}

IHook::~IHook()
{
}

const Hook *IHook::GetHook() const
{
   return &hHook;
}

IHook::operator const Hook* () const
{
   return &hHook;
}

