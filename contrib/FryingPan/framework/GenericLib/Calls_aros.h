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

#include <aros/libcall.h>

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

#define PROC0(name,off) \
   void name()                                     \
   {                                               \
   AROS_LVO_CALL0(BOOL, struct Library*, LibBase, off,0);\
   }
   
#define PROC1(name,off,type1,parm1,reg1) \
   void name(type1 parm1)                          \
   {                                               \
   AROS_LVO_CALL1(BOOL, AROS_LCA(type1, parm1, reg1), struct Library*, LibBase, off,0);\
   }

#define PROC2(name,off,type1,parm1,reg1,type2,parm2,reg2) \
   void name(type1 parm1, type2 parm2)             \
   {                                               \
   AROS_LVO_CALL2(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), struct Library*, LibBase, off,0);\
   }

#define PROC3(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   void name(type1 parm1, type2 parm2, type3 parm3)                 \
   {                                               \
   AROS_LVO_CALL3(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), struct Library*, LibBase, off,0);\
   }

#define PROC4(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)           \
   {                                               \
   AROS_LVO_CALL4(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), struct Library*, LibBase, off,0);\
   }

#define PROC5(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)    \
   {                                               \
   AROS_LVO_CALL5(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), struct Library*, LibBase, off,0);\
   }

#define PROC6(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6) \
   {                                               \
   AROS_LVO_CALL6(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), struct Library*, LibBase, off,0);\
   }

#define PROC7(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7) \
   {                                               \
      AROS_LVO_CALL7(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), struct Library*, LibBase, off,0);\
   }
   
#define PROC8(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8) \
   {                                               \
      AROS_LVO_CALL8(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), struct Library*, LibBase, off,0);\
   }
   
#define PROC9(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9) \
   {                                               \
      AROS_LVO_CALL9(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), AROS_LCA(type9, parm9, reg9), struct Library*, LibBase, off,0);\
   }
   
#define PROC10(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10) \
   {                                               \
      AROS_LVO_CALL10(BOOL, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), AROS_LCA(type9, parm9, reg9), AROS_LCA(type10, parm10, reg10), struct Library*, LibBase, off,0);\
   }
   
#define PROC11(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11) \
   {                                               \
      ((void (*)(type1,type2,type3,type4,type5,type6,type7,type8,type9,type10,type11,Library*))    \
       (__AROS_GETVECADDR(LibBase, off)))                                                 \
       (parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10, parm11, LibBase);   \
   }
   
#define FUNC0(type, name, off) \
   type name()                                     \
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL0(type, struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC1(type, name,off,type1,parm1,reg1)  \
   type name(type1 parm1)                          \
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL1(type, AROS_LCA(type1, parm1, reg1), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC2(type, name,off,type1,parm1,reg1,type2,parm2,reg2)  \
   type name(type1 parm1, type2 parm2)             \
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL2(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC3(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   type name(type1 parm1, type2 parm2, type3 parm3)\
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL3(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC4(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL4(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC5(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL5(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC6(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL6(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC7(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL7(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), struct Library*, LibBase, off,0);\
      return Result;                               \
   }

#define FUNC8(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8) \
   {                                               \
      type Result;                                 \
      Result = AROS_LVO_CALL8(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), struct Library*, LibBase, off,0);\
      return Result;                               \
   }
   
#define FUNC9(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9) \
   {                                               \
      type Result;                                 \
      AROS_LVO_CALL9(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), AROS_LCA(type9, parm9, reg9), struct Library*, LibBase, off,0);\
      return Result;                               \
   }
   
#define FUNC10(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10) \
   {                                               \
      type Result;                                 \
      AROS_LVO_CALL10(type, AROS_LCA(type1, parm1, reg1), AROS_LCA(type2, parm2, reg2), AROS_LCA(type3, parm3, reg3), AROS_LCA(type4, parm4, reg4), AROS_LCA(type5, parm5, reg5), AROS_LCA(type6, parm6, reg6), AROS_LCA(type7, parm7, reg7), AROS_LCA(type8, parm8, reg8), AROS_LCA(type9, parm9, reg9), AROS_LCA(type10, parm10, reg10), struct Library*, LibBase, off,0);\
      return Result;                               \
   }
   
#define FUNC11(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11) \
   {                                               \
      return                                       \
      ((type (*)(type1,type2,type3,type4,type5,type6,type7,type8,type9,type10,type11))    \
       (__AROS_GETVECADDR(LibBase, off)))                                                 \
       (parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10, parm11);   \
   }

