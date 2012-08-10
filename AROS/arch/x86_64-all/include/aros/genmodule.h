#ifndef AROS_X86_64_GENMODULE_H
#define AROS_X86_64_GENMODULE_H

/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$

    Desc: genmodule specific definitions for x86_64
    Lang: english
*/

#include <exec/execbase.h>

/* Macros for generating library stub functions and aliases for stack libcalls. */

/* Macro for generating the __GM_GetBase() function
 * for a library. Optional for most architectures,
 * but we can save quite a bit of pushing and popping
 * with this optimized version.
 *
 * Pseudocode:
 * 
 * return SysBase->ThisTask->tc_UnionETask.tc_ETask.et_TaskStorage[__GM_BaseSlot];
 *
 * We can only munge %r11
 * Base is in %r11 on exit
 */
#define __AROS_GM_GETBASE() \
    asm volatile (  "    movq   SysBase(%%rip), %%r11\n"  \
                    "    movq   %c[task](%%r11), %%r11\n"  \
                    "    movq   %c[etask](%%r11), %%r11\n"  \
                    "    movq   %c[ts](%%r11), %%r11\n"  \
                    "    shrq   $3, %%r11\n" \
                    "    addq   __GM_BaseSlot(%%rip),%%r11\n"  \
                    "    shlq   $3, %%r11\n" \
                    "    movq   (%%r11), %%r11\n" \
                 : : [task] "i"(offsetof(struct ExecBase, ThisTask)), \
                     [etask] "i"(offsetof(struct Task, tc_UnionETask.tc_ETask)), \
                     [ts] "i"(offsetof(struct ETask, et_TaskStorage)));

#define __AROS_GM_SETBASE() \
    asm volatile (  "    pushq  %%rax\n" \
                    "    movq   SysBase(%%rip), %%rax\n"  \
                    "    movq   %c[task](%%rax), %%rax\n"  \
                    "    movq   %c[etask](%%rax), %%rax\n"  \
                    "    movq   %c[ts](%%rax), %%rax\n"  \
                    "    shrq   $3, %%rax\n" \
                    "    add    __GM_BaseSlot(%%rip),%%rax\n"  \
                    "    shlq   $3, %%rax\n" \
                    "    movq   %%r11,(%%rax)\n" \
                    "    popq   %%rax\n" \
                 : : [task] "i"(offsetof(struct ExecBase, ThisTask)), \
                     [etask] "i"(offsetof(struct Task, tc_UnionETask.tc_ETask)), \
                     [ts] "i"(offsetof(struct ETask, et_TaskStorage)));

/* Macro: AROS_GM_LIBFUNCALIAS(functionname, alias)
   This macro will generate an alias 'alias' for function
   'functionname'
*/
#define __AROS_GM_LIBFUNCALIAS(fname, alias) \
    asm(".weak " #alias "\n" \
	"\t.set " #alias "," #fname \
    );
#define AROS_GM_LIBFUNCALIAS(fname, alias) \
    __AROS_GM_LIBFUNCALIAS(fname, alias)


/* Macro: AROS_GM_LIBFUNCSTUB(functionname, libbasename, lvo)
   This macro will generate code for a stub function for
   the function 'functionname' of library with libbase
   'libbasename' and 'lvo' number of the function in the
   vector table. lvo has to be a constant value (not a variable)

   Internals: a dummy function is used that will generate some
   unused junk code but otherwise we can't pass input arguments
   to the asm statement
*/
#define __AROS_GM_LIBFUNCSTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _wrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n" \
            #fname " :\n" \
            "\tmovq " #libbasename "(%%rip), %%r11\n" \
            "\tjmp  *%c0(%%r11)\n" \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }
#define AROS_GM_LIBFUNCSTUB(fname, libbasename, lvo) \
    __AROS_GM_LIBFUNCSTUB(fname, libbasename, lvo)

/* Macro: AROS_GM_RELLIBFUNCSTUB(functionname, libbasename, lvo)
   Same as AROS_GM_LIBFUNCSTUB but finds libbase at an offset in
   the current libbase
*/
#define __AROS_GM_RELLIBFUNCSTUB(fname, libbasename, lvo) \
    void __ ## fname ## _ ## libbasename ## _relwrapper(void) \
    { \
        asm volatile( \
            ".weak " #fname "\n" \
            "\t" #fname " :\n"); \
        __AROS_GM_GETBASE() \
        asm volatile( \
            "\taddq __GM_" #libbasename "_offset(%%rip), %%r11\n" \
            "\tmovq (%%r11),%%r11\n" \
            "\tjmp  *%c0(%%r11)\n" \
            : : "i" ((-lvo)*LIB_VECTSIZE) \
        ); \
    }
#define AROS_GM_RELLIBFUNCSTUB(fname, libbasename, lvo) \
    __AROS_GM_RELLIBFUNCSTUB(fname, libbasename, lvo)

#define __GM_STRINGIZE(x) #x
#define __AROS_GM_STACKCALL(fname, libbasename, libfuncname) \
    void libfuncname(void); \
    void __ ## fname ## _stackcall(void) \
    { \
        asm volatile( \
            "\t" __GM_STRINGIZE(libfuncname) " :\n"); \
            __AROS_GM_SETBASE(); /* preserve %r11 in this library's slot */ \
        asm volatile( \
            "\tjmp  " #fname "\n" \
        ); \
    }
    
#define AROS_GM_STACKCALL(fname, libbasename, lvo) \
     __AROS_GM_STACKCALL(fname, libbasename, AROS_SLIB_ENTRY(fname, libbasename, lvo))

/* Macro: AROS_GM_STACKALIAS(functionname, libbasename, lvo)
   This macro will generate an alias 'alias' for function
   'functionname'
*/
#define __AROS_GM_STACKALIAS(fname, alias) \
    void alias(void); \
    asm(".weak " __GM_STRINGIZE(alias) "\n" \
        "\t.set " __GM_STRINGIZE(alias) "," #fname \
    );
#define AROS_GM_STACKALIAS(fname, libbasename, lvo) \
    __AROS_GM_STACKALIAS(fname, AROS_SLIB_ENTRY(fname, libbasename, lvo))


#endif /* AROS_X86_64_GENMODULE_H */
