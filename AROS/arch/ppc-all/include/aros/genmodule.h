#ifndef AROS_PPC_GENMODULE_H
#define AROS_PPC_GENMODULE_H
/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$

    NOTE: This file must compile *without* any other header !

    Desc: genmodule.h include file for powerpc arch
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
 * return SysBase->ThisTask->tc_UnionETask.tc_ETask->et_TaskStorage[__GM_BaseSlot];
 *
 * We can only use registers %r0, %r11, and %r12
 * On exit, %r12 will have the base or NULL
 */
#define __AROS_GM_GETBASE() \
    asm volatile ( \
            "    lis   %%r11,SysBase@ha\n" \
            "    lwz   %%r11,SysBase@l(%%r11)\n" \
            "    lwz   %%r11,%c[task](%%r11)\n" \
            "    lwz   %%r11,%c[etask](%%r11)\n" \
            "    lwz   %%r11,%c[ts](%%r11)\n" \
            "    lis   %%r12,__GM_BaseSlot@ha\n" \
            "    lwz   %%r12,__GM_BaseSlot@l(%%r12)\n" \
            "    slwi  %%r12,%%r12,2\n" \
            "    lwzx  %%r12,%%r11,%%r12\n" \
            "1:\n" \
                 : : [task] "i"(offsetof(struct ExecBase, ThisTask)), \
                     [etask] "i"(offsetof(struct Task, tc_UnionETask.tc_ETask)), \
                     [ts] "i"(offsetof(struct ETask, et_TaskStorage)));


#define __AROS_GM_SETBASE() \
    asm volatile ( \
            "    lis   %%r11,__GM_BaseSlot@ha\n" \
            "    lwz   %%r11,__GM_BaseSlot@l(%%r11)\n" \
            "    slwi  %%r11,%%r11,2\n" \
            "    mr    %%r0,%%r11\n" \
            "    lis   %%r11,SysBase@ha\n" \
            "    lwz   %%r11,SysBase@l(%%r11)\n" \
            "    lwz   %%r11,%c[task](%%r11)\n" \
            "    lwz   %%r11,%c[etask](%%r11)\n" \
            "    lwz   %%r11,%c[ts](%%r11)\n" \
            "    stwx  %%r12,%%r11,%%r0\n" \
            "1:\n" \
                 : : [task] "i"(offsetof(struct ExecBase, ThisTask)), \
                     [etask] "i"(offsetof(struct Task, tc_UnionETask.tc_ETask)), \
                     [ts] "i"(offsetof(struct ETask, et_TaskStorage)));


/* Macro: AROS_GM_LIBFUNCSTUB(functionname, libbasename, lvo)
   This macro will generate code for a stub function for
   the function 'functionname' of lirary with libbase
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
	    "\t" #fname ":\n" \
	    "\tlis   %%r12," #libbasename "@ha\n" \
	    "\tlwz   %%r12," #libbasename "@l(%%r12)\n" \
	    "\tlwz   %%r11,%c0(%%r12)\n" \
	    "\tmtctr %%r11\n" \
	    "\tbctr\n" \
	    : : "i" ((-lvo*LIB_VECTSIZE)) \
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
	    "\t" #fname ":\n" \
        ); \
	    __AROS_GM_GETBASE() \
	asm volatile( \
	    "\tlis  %%r11,__GM_" #libbasename "_offset@ha\n" \
	    "\tlwz  %%r11,__GM_" #libbasename "_offset@l(%%r11)\n" \
            "\tlwzx %%r12, %%r12, %%r11\n" \
	    "\tlis  %%r11, %c0@ha\n" \
	    "\tlwz  %%r11, %c0@l(%%r11)\n" \
	    "\tadd  %%r11, %%r12, %%r11\n" \
	    "\tmtctr %%r11\n" \
	    "\tbctr\n" \
	    : : "i" ((-lvo*LIB_VECTSIZE)) \
        ); \
    }
#define AROS_GM_RELLIBFUNCSTUB(fname, libbasename, lvo) \
    __AROS_GM_RELLIBFUNCSTUB(fname, libbasename, lvo)

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

/******************* Library Side Thunks ******************/

/* This macro relies upon the fact that the
 * caller to a stack function will have passed in
 * the base in %r12, since the caller
 * will have used the AROS_GM_LIBFUNCSTUB() macro.
 */
#define __GM_STRINGIZE(x) #x
#define __AROS_GM_STACKCALL(fname, libbasename, libfuncname) \
    void libfuncname(void); \
    void __ ## fname ## _stackcall(void) \
    { \
        asm volatile( \
            "\t" __GM_STRINGIZE(libfuncname) " :\n" \
        ); \
        __AROS_GM_SETBASE(); \
        asm volatile( \
            "\tb " #fname "\n"  \
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


#endif /* AROS_PPC_GENMODULE_H */
