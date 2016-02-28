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

#ifndef _LIBWRAPPER_H_
#define _LIBWRAPPER_H_

#include <exec/libraries.h>
#include <Generic/Types.h>

struct LibInitStruct
{
   unsigned long  LibSize;
   void          *FuncTable;
   void          *DataTable;
   void           (*InitFunc)(void);
};


#ifdef __cplusplus
extern "C"
{
#endif 

   struct Library               *LIB_Init(struct Library *pOurBase, void *pSegList, struct ExecBase *pSysBase);
   struct Library               *LIB_Open(void);
   IPTR                          LIB_Close(void);
   IPTR                          LIB_Expunge(void);
   IPTR                          LIB_Reserved(void);
   extern struct LibInitStruct   LIB_InitStruct;
#ifdef __cplusplus
};
#endif

#define LIBRARY(name, id, version)              \
   struct Resident LIB_Resident =               \
   {                                            \
      RTC_MATCHWORD,                            \
      &LIB_Resident,                            \
      &LIB_Resident+1,                          \
      RTF_AUTOINIT,                             \
      version,                                  \
      NT_LIBRARY,                               \
      0,                                        \
      name,                                     \
      id,                                       \
      &LIB_InitStruct,                          \
   };      

#define LIB_FT_Begin                            \
   uint LIB_FuncTable[] =                       \
   {                                            \
      (uint) &LIB_Open,                         \
      (uint) &LIB_Close,                        \
      (uint) &LIB_Expunge,                      \
      (uint) &LIB_Reserved,
// }

#define LIB_FT_Function(f) (uint) &f,

// { LIB_FT_End
#define LIB_FT_End                              \
      0xffffffff,                               \
   };

#define GATE0(type, name)                       \
   type name();

#define GATE1(type, name, type1, reg1)          \
   type name(type1);

#define GATE2(type, name, type1, reg1, type2, reg2) \
   type name(type1, type2);

#define GATE3(type, name, type1, reg1, type2, reg2, type3, reg3) \
   type name(type1, type2, type3);

#define GATE4(type, name, type1, reg1, type2, reg2, type3, reg3, type4, reg4) \
   type name(type1, type2, type3, type4);

#define GATE5(type, name, type1, reg1, type2, reg2, type3, reg3, type4, reg4, type5, reg5) \
   type name(type1, type2, type3, type4, type5);




#endif //_LIBWRAPPER_H_
