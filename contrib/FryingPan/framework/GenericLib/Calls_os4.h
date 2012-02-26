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

#include <amiga_compiler.h>
#include <exec/interfaces.h>

#define BEGINSHORTDECL(name)           \
   struct name##IFace                  \
   {                                   \
      struct InterfaceData Data;       \
      void (*Reserved1)();             \
      void (*Reserved2)();             \
      void (*Reserved3)();             \
      void (*Reserved4)();                               

#define BEGINDECL(name, libname)                \
   struct name##IFace                           \
   {                                            \
      struct InterfaceData Data;                         \
      static name##IFace* GetInstance(short version)     \
      {                                                  \
         Library *pLib = Exec->OpenLibrary(libname, version);  \
         if (0 == pLib) return 0;                        \
         return (name##IFace*)Exec->GetInterface(pLib, "main", 1, 0);   \
      }                                                  \
                                                         \
      void FreeInstance()                                \
      {                                                  \
         Library *pLib = Data.LibBase;                   \
         Exec->DropInterface((Interface*)this);         \
         Exec->CloseLibrary(pLib);                      \
      }                                                  \
                                                         \
      void (*Reserved1)();                               \
      void (*Reserved2)();                               \
      void (*Reserved3)();                               \
      void (*Reserved4)();                               

#define ENDDECL                                          \
   };                                           


#define PROC0(name, off)                                 \
   void name()                                           \
   {                                                     \
      (direct##name)((Interface*)this);                  \
   }                                                     \
   void (*direct##name)(Interface* p1);          

#define PROC1(name,off,type1,parm1,reg1) \
   void name(type1 parm1)                                \
   {                                                     \
      (direct##name)((Interface*)this, parm1);           \
   }                                                     \
   void (*direct##name)(Interface* p1, type1 parm1);          

#define PROC2(name,off,type1,parm1,reg1,type2,parm2,reg2) \
   void name(type1 parm1, type2 parm2)                   \
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2);    \
   }                                                     \
   void (*direct##name)(Interface* p1, type1 parm1, type2 parm2);          

#define PROC3(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   void name(type1 parm1, type2 parm2, type3 parm3)      \
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3);\
   }                                                     \
   void (*direct##name)(Interface* p1, type1 parm1, type2 parm2, type3 parm3);

#define PROC4(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4);

#define PROC5(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5);

#define PROC6(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6);

#define PROC7(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7);
   
#define PROC8(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8);
   
#define PROC9(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9);
   
#define PROC10(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10)\
   {                                                     \
      (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10);\
   }                                                     \
   void (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10);
   
#define FUNC0(type, name, off) \
   type name()                                           \
   {                                                     \
      return (direct##name)((Interface*)this);           \
   }                                                     \
   type (*direct##name)(Interface* p1);          

#define FUNC1(type, name,off,type1,parm1,reg1)  \
   type name(type1 parm1)                                \
   {                                                     \
      return (direct##name)((Interface*)this, parm1);    \
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1);          

#define FUNC2(type, name,off,type1,parm1,reg1,type2,parm2,reg2)  \
   type name(type1 parm1, type2 parm2)                   \
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2);          

#define FUNC3(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   type name(type1 parm1, type2 parm2, type3 parm3)      \
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3);

#define FUNC4(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4);

#define FUNC5(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5);

#define FUNC6(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6);

#define FUNC7(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7);

#define FUNC8(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8);

#define FUNC9(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9);

#define FUNC10(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10);

#define FUNC11(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11)\
   {                                                     \
      return (direct##name)((Interface*)this, parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10, parm11);\
   }                                                     \
   type (*direct##name)(Interface *p1, type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11);

