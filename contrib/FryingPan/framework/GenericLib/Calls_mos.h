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


/*
 * due to different handling of certain functions on various packages
 * a flag has been introduced. now:
 * MOS_DIRECT_OS
 * needs to be set for emulhandle directos calls.
 */

#define BEGINSHORTDECL(name)           \
   struct name##IFace                  \
   {                                   \
      struct Library   *LibBase;

#define BEGINDECL(name, libname)                \
   struct name##IFace                           \
   {                                            \
      struct Library *LibBase;                  \
      static name##IFace* GetInstance(short version) \
      {                                         \
         Library* pBase = Exec->OpenLibrary(libname, version);\
         if (pBase == 0) return 0;              \
         name##IFace *pFace = new name##IFace;  \
         pFace->LibBase = pBase;                \
         return pFace;                          \
      }                                         \
                                                \
      void FreeInstance()                       \
      {                                         \
         Exec->CloseLibrary(LibBase);          \
         delete this;                           \
      }

#define ENDDECL                                 \
   };                                           

 

#if defined(MOS_DIRECT_OS)
#include <ppcinline/macros.h>




#define PROC0(name, off)                                    \
   void name()                                              \
   {                                                        \
      LP0NR(6*off, 0, 0, LibBase, 0, 0, 0, 0, 0, 0);        \
   }

#define PROC1(name,off,type1,parm1,reg1)                    \
   void name(type1 parm1)                                   \
   {                                                        \
      LP1NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC2(name,off,type1,parm1,reg1,type2,parm2,reg2)   \
   void name(type1 parm1, type2 parm2)                      \
   {                                                        \
      LP2NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC3(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   void name(type1 parm1, type2 parm2, type3 parm3)         \
   {                                                        \
      LP3NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC4(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)           \
   {                                                        \
      LP4NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC5(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)    \
   {                                                        \
      LP5NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC6(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6) \
   {                                                        \
      LP6NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define PROC7(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7) \
   {                                                        \
      LP7NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }
   
#define PROC8(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8) \
   {                                                        \
      LP8NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }
   
#define PROC9(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9) \
   {                                                        \
      LP9NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            type9, parm9, reg9,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }
   
#define PROC10(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10) \
   {                                                        \
      LP10NR(6*off, 0,                                       \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            type9, parm9, reg9,                             \
            type10, parm10, reg10,                          \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }
   
#define FUNC0(type, name, off) \
   type name()                                              \
   {                                                        \
      return LP0(6*off, type, 0,                            \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC1(type, name,off,type1,parm1,reg1)              \
   type name(type1 parm1)                                   \
   {                                                        \
      return LP1(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC2(type, name,off,type1,parm1,reg1,type2,parm2,reg2)  \
   type name(type1 parm1, type2 parm2)                      \
   {                                                        \
      return LP2(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC3(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   type name(type1 parm1, type2 parm2, type3 parm3)         \
   {                                                        \
      return LP3(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC4(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                                        \
      return LP4(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC5(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                                        \
      return LP5(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC6(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                                        \
      return LP6(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC7(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                                        \
      return LP7(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC8(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8)\
   {                                                        \
      return LP8(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC9(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9)\
   {                                                        \
      return LP9(6*off, type, 0,                            \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            type9, parm9, reg9,                             \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC10(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10)\
   {                                                        \
      return LP10(6*off, type, 0,                           \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            type9, parm9, reg9,                             \
            type10,parm10,reg10,                            \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#define FUNC11(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11)\
   {                                                        \
      return LP11(6*off, type, 0,                           \
            type1, parm1, reg1,                             \
            type2, parm2, reg2,                             \
            type3, parm3, reg3,                             \
            type4, parm4, reg4,                             \
            type5, parm5, reg5,                             \
            type6, parm6, reg6,                             \
            type7, parm7, reg7,                             \
            type8, parm8, reg8,                             \
            type9, parm9, reg9,                             \
            type10,parm10,reg10,                            \
            type11,parm11,reg11,                            \
            0, LibBase, 0, 0, 0, 0, 0, 0);                  \
   }

#else




#define PROC0(name, off) \
   void name()                                     \
   {                                               \
   ((void (*)())*(void**)((long)LibBase-(6*off-2)))();\
   }

#define PROC1(name,off,type1,parm1,reg1) \
   void name(type1 parm1)                          \
   {                                               \
   ((void (*)(type1))*(void**)((long)LibBase-(6*off-2)))(parm1);\
   }

#define PROC2(name,off,type1,parm1,reg1,type2,parm2,reg2) \
   void name(type1 parm1, type2 parm2)             \
   {                                               \
   ((void (*)(type1, type2))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2);\
   }

#define PROC3(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   void name(type1 parm1, type2 parm2, type3 parm3)                 \
   {                                               \
   ((void (*)(type1, type2, type3))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3);\
   }

#define PROC4(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)           \
   {                                               \
   ((void (*)(type1, type2, type3, type4))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4);\
   }

#define PROC5(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)    \
   {                                               \
   ((void (*)(type1, type2, type3, type4, type5))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5);\
   }

#define PROC6(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6) \
   {                                               \
   ((void (*)(type1, type2, type3, type4, type5, type6))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6);\
   }

#define PROC7(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7) \
   {                                               \
   ((void (*)(type1, type2, type3, type4, type5, type6, type7))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6, parm7);\
   }
   
#define FUNC0(type, name, off) \
   type name()                                     \
   {                                               \
   return ((type (*)())*(void**)((long)LibBase-(6*off-2)))();\
   }

#define FUNC1(type, name,off,type1,parm1,reg1)  \
   type name(type1 parm1)                          \
   {                                               \
   return ((type (*)(type1))*(void**)((long)LibBase-(6*off-2)))(parm1);\
   }

#define FUNC2(type, name,off,type1,parm1,reg1,type2,parm2,reg2)  \
   type name(type1 parm1, type2 parm2)             \
   {                                               \
   return ((type (*)(type1, type2))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2);\
   }

#define FUNC3(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   type name(type1 parm1, type2 parm2, type3 parm3)\
   {                                               \
   return ((type (*)(type1, type2, type3))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3);\
   }

#define FUNC4(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4);\
   }

#define FUNC5(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4, type5))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5);\
   }

#define FUNC6(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4, type5, type6))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6);\
   }

#define FUNC7(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4, type5, type6, type7))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6, parm7);\
   }

#define FUNC8(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4, type5, type6, type7))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8);\
   }

#define FUNC9(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9)\
   {                                               \
   return ((type (*)(type1, type2, type3, type4, type5, type6, type7, type8, type9))*(void**)((long)LibBase-(6*off-2)))(parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9);\
   }


#endif /* MOS_DIRECT_OS */

