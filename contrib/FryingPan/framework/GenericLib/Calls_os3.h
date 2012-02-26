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

#ifdef __stringify
#undef __stringify
#endif
#define __stringify(X) #X

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
   


#define PROC0(name, off)                        \
   void name()                                     \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase)                         \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", "memory");\
   }

#define PROC1(name,off,type1,parm1,reg1) \
   void name(type1 parm1)                          \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), "memory");\
   }

#define PROC2(name,off,type1,parm1,reg1,type2,parm2,reg2) \
   void name(type1 parm1, type2 parm2)             \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")\n" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), "memory");\
   }
            

#define PROC3(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   void name(type1 parm1, type2 parm2, type3 parm3)                 \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), "memory");\
   }

#define PROC4(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)           \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), "memory");\
   }

#define PROC5(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)    \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), "memory");\
   }

#define PROC6(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), "memory");\
   }

#define PROC7(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "mov.l   %7," __stringify(reg7) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), "memory");\
   }

#define PROC8(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "mov.l   %7," __stringify(reg7) "\n\t" \
            "mov.l   %8," __stringify(reg8) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), "memory");\
   }

#define PROC9(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "mov.l   %7," __stringify(reg7) "\n\t" \
            "mov.l   %8," __stringify(reg8) "\n\t" \
            "mov.l   %9," __stringify(reg9) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), "memory");\
   }

#define PROC10(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "mov.l   %7," __stringify(reg7) "\n\t" \
            "mov.l   %8," __stringify(reg8) "\n\t" \
            "mov.l   %9," __stringify(reg9) "\n\t" \
            "mov.l   %10," __stringify(reg10) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9),                          \
              "ginsm"(parm10)                          \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), __stringify(reg10), "memory");\
   }

#define PROC11(name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   void name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11) \
   {                                               \
   asm volatile(     "mov.l   %0,a6\n\t"                    \
            "mov.l   %1," __stringify(reg1) "\n\t" \
            "mov.l   %2," __stringify(reg2) "\n\t" \
            "mov.l   %3," __stringify(reg3) "\n\t" \
            "mov.l   %4," __stringify(reg4) "\n\t" \
            "mov.l   %5," __stringify(reg5) "\n\t" \
            "mov.l   %6," __stringify(reg6) "\n\t" \
            "mov.l   %7," __stringify(reg7) "\n\t" \
            "mov.l   %8," __stringify(reg8) "\n\t" \
            "mov.l   %9," __stringify(reg9) "\n\t" \
            "mov.l   %10," __stringify(reg10) "\n\t" \
            "mov.l   %11," __stringify(reg11) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            :                                      \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9),                          \
              "ginsm"(parm10),                         \
              "ginsm"(parm11)                          \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), __stringify(reg10), __stringify(reg11), "memory");\
   }

#define FUNC0(type, name, off) \
   type name()                                     \
   {                                               \
   register type Result asm ("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase)                         \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", "memory");\
   return Result;                                  \
   }

#define FUNC1(type, name,off,type1,parm1,reg1)  \
   type name(type1 parm1)                          \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile (     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), "memory");\
   return Result;                                  \
   }

#define FUNC2(type, name,off,type1,parm1,reg1,type2,parm2,reg2)  \
   type name(type1 parm1, type2 parm2)             \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," #reg1 "\n\t"             \
            "mov.l   %3," #reg2 "\n\t"             \
            "jbsr    a6@(-6*" #off ")"             \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), "memory"); \
   return Result;                                  \
   }

#define FUNC3(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3) \
   type name(type1 parm1, type2 parm2, type3 parm3)\
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), "memory");\
   return Result;                                  \
   }

#define FUNC4(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4)\
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), "memory");\
   return Result;                                  \
   }

#define FUNC5(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5)\
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), "memory");\
   return Result;                                  \
   }

#define FUNC6(type, name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6)\
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), "memory");\
   return Result;                                  \
   }

#define FUNC7(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7)\
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "mov.l   %8," __stringify(reg7) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), "memory");\
   return Result;                                  \
   }

#define FUNC8(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8) \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "mov.l   %8," __stringify(reg7) "\n\t" \
            "mov.l   %9," __stringify(reg8) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), "memory");\
   return Result;                                  \
   }

#define FUNC9(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9) \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "mov.l   %8," __stringify(reg7) "\n\t" \
            "mov.l   %9," __stringify(reg8) "\n\t" \
            "mov.l   %10," __stringify(reg9) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9)                           \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), "memory");\
   return Result;                                  \
   }

#define FUNC10(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10) \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "mov.l   %8," __stringify(reg7) "\n\t" \
            "mov.l   %9," __stringify(reg8) "\n\t" \
            "mov.l   %10," __stringify(reg9) "\n\t" \
            "mov.l   %11," __stringify(reg10) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9),                          \
              "ginsm"(parm10)                          \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), __stringify(reg10), "memory");\
   return Result;                                  \
   }

#define FUNC11(type,name,off,type1,parm1,reg1,type2,parm2,reg2,type3,parm3,reg3,type4,parm4,reg4,type5,parm5,reg5,type6,parm6,reg6,type7,parm7,reg7,type8,parm8,reg8,type9,parm9,reg9,type10,parm10,reg10,type11,parm11,reg11) \
   type name(type1 parm1, type2 parm2, type3 parm3, type4 parm4, type5 parm5, type6 parm6, type7 parm7, type8 parm8, type9 parm9, type10 parm10, type11 parm11) \
   {                                               \
   register type Result asm("d0");                 \
   asm volatile(     "mov.l   %1,a6\n\t"                    \
            "mov.l   %2," __stringify(reg1) "\n\t" \
            "mov.l   %3," __stringify(reg2) "\n\t" \
            "mov.l   %4," __stringify(reg3) "\n\t" \
            "mov.l   %5," __stringify(reg4) "\n\t" \
            "mov.l   %6," __stringify(reg5) "\n\t" \
            "mov.l   %7," __stringify(reg6) "\n\t" \
            "mov.l   %8," __stringify(reg7) "\n\t" \
            "mov.l   %9," __stringify(reg8) "\n\t" \
            "mov.l   %10," __stringify(reg9) "\n\t" \
            "mov.l   %11," __stringify(reg10) "\n\t" \
            "mov.l   %12," __stringify(reg11) "\n\t" \
            "jbsr    a6@(-6*" __stringify(off) ")" \
            : "=r"(Result)                         \
            : "ginsm"(LibBase),                        \
              "ginsm"(parm1),                          \
              "ginsm"(parm2),                          \
              "ginsm"(parm3),                          \
              "ginsm"(parm4),                          \
              "ginsm"(parm5),                          \
              "ginsm"(parm6),                          \
              "ginsm"(parm7),                          \
              "ginsm"(parm8),                          \
              "ginsm"(parm9),                          \
              "ginsm"(parm10),                         \
              "ginsm"(parm11)                          \
            : "d0", "d1", "a0", "a1", "fp0", "fp1", "a6", "cc", __stringify(reg1), __stringify(reg2), __stringify(reg3), __stringify(reg4), __stringify(reg5), __stringify(reg6), __stringify(reg7), __stringify(reg8), __stringify(reg9), __stringify(reg10), __stringify(reg11), "memory");\
   return Result;                                  \
   }

