#ifndef FEELIN_MACHINE_H
#define FEELIN_MACHINE_H

/*
**  feelin/machine.h
**
**  Must be included before any other Feelin include.
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 01.04 (2005/08/09)

    Moved API macros from Feelin:Sources/feelin/Private.h here.

$VER: 01.00 (2005/06/21)

*/

#ifdef __MORPHOS__

///Gate macros

#include <emul/emulinterface.h>

#ifndef __GNUC__
#error "This header is designed to be used with GCC only on MorphOS platform."
#endif

#define _GATENAME(name)                         _GTE_##name
#define _GATEPROTO(name)                        const struct EmulLibEntry _GATENAME(name)
#define _MAKEGATE(name, fct)                    _GATEPROTO(name) __attribute__((unused)) = {TRAP_LIB, 0, (APTR) (fct)}
#define _MAKEGATENR(name, fct)                  _GATEPROTO(name) __attribute__((unused)) = {TRAP_LIBNR, 0, (APTR) (fct)}

#define _DEFFUNC0(name, rt) \
    static rt _PPC_##name(void); \
    rt _JMP_##name(void) \
    { return _PPC_##name(); } \
    _MAKEGATE(name, _JMP_##name); \
    rt (*name)(void) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static rt _PPC_##name(void)

#define _DEFFUNC2(name, rt, r1, t1, v1, r2, t2, v2) \
    static rt _PPC_##name(t1 v1, t2 v2); \
    rt _JMP_##name(void) \
    { return _PPC_##name((t1) REG_##r1, (t2) REG_##r2); } \
    _MAKEGATE(name, _JMP_##name); \
    rt (*name)(t1 v1, t2 v2) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static rt _PPC_##name(t1 v1, t2 v2)

#define _DEFFUNC2NR(name, r1, t1, v1, r2, t2, v2) \
    static void _PPC_##name(t1 v1, t2 v2); \
    void _JMP_##name(void) \
    { _PPC_##name((t1) REG_##r1, (t2) REG_##r2); } \
    _MAKEGATENR(name, _JMP_##name); \
    void (*name)(t1 v1, t2 v2) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static void _PPC_##name(t1 v1, t2 v2)

#define _DEFFUNC3(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3) \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3); \
    rt _JMP_##name(void) \
    { return _PPC_##name((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3); } \
    _MAKEGATE(name, _JMP_##name); \
    rt (*name)(t1 v1, t2 v2, t3 v3) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3)

#define _DEFFUNC4(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3, t4 v4); \
    rt _JMP_##name(void) \
    { return _PPC_##name((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4); } \
    _MAKEGATE(name, _JMP_##name); \
    rt (*name)(t1 v1, t2 v2, t3 v3, t4 v4) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3, t4 v4)

#define _DEFFUNC5(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5) \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5); \
    rt _JMP_##name(void) \
    { return _PPC_##name((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (t5) REG_##r5); } \
    _MAKEGATE(name, _JMP_##name); \
    rt (*name)(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) __attribute__((unused)) = (APTR) &_GATENAME(name); \
    static rt _PPC_##name(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5)

#define _LIB_DEFFUNC1(name, rt, r1, t1, v1)     _DEFFUNC2(name, rt, r1, t1, v1, a6, struct FeelinBase *, FeelinBase)
#define _LIB_DEFFUNC1NR(name, r1, t1, v1)       _DEFFUNC2NR(name, r1, t1, v1, a6, struct FeelinBase *, FeelinBase)

#define _LIB_DEFFUNC2NR(name, r1, t1, v1, r2, t2, v2) \
    _DEFFUNC3NR(name, r1, t1, v1, r2, t2, v2, a6, struct FeelinBase *, FeelinBase)

#define _LIB_DEFFUNC4(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) \
    _DEFFUNC5(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, a6, struct FeelinBase *, FeelinBase)

//+

/* F_FUNCTION_GATE gives the right function to call between code (or
machine code) */

#define F_FUNCTION_GATE(name)                   ((APTR) &_GATENAME(name))
 
#else /*** classic ***/

#define F_FUNCTION_GATE(name)                   name

#endif

/// Public library functions definition and prototyping macros

/*

    You must define a F_LIB_STRUCT_AND_BASE macro to use your library  base
    e.g.

#define F_LIB_STRUCT_AND_BASE                   struct in_FeelinBase *FeelinBase

*/

#define LIB_PROTO0(name, rt) ASM(rt) name(REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO0NR(name) ASM(void) name(REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO1(name, rt, r1, v1) ASM(rt) name(REG(r1, v1), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO1NR(name, r1, v1) ASM(void) name(REG(r1, v1), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO2(name, rt, r1, v1, r2, v2) ASM(rt) name(REG(r1, v1), REG(r2, v2), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO2NR(name, r1, v1, r2, v2) ASM(void) name(REG(r1, v1), REG(r2, v2), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO3(name, rt, r1, v1, r2, v2, r3, v3) ASM(rt) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO3NR(name, r1, v1, r2, v2, r3, v3) ASM(void) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO4(name, rt, r1, v1, r2, v2, r3, v3, r4, v4) ASM(rt) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO4NR(name, r1, v1, r2, v2, r3, v3, r4, v4) ASM(void) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO5(name, rt, r1, v1, r2, v2, r3, v3, r4, v4, r5, v5) ASM(rt) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(r5, v5), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO5NR(name, r1, v1, r2, v2, r3, v3, r4, v4, r5, v5) ASM(void) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(r5, v5), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO6(name, rt, r1, v1, r2, v2, r3, v3, r4, v4, r5, v5, r6, v6) ASM(rt) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(r5, v5), REG(r6, v6), REG(a6, F_LIB_STRUCT_AND_BASE))
#define LIB_PROTO6NR(name, r1, v1, r2, v2, r3, v3, r4, v4, r5, v5, r6, v6) ASM(void) name(REG(r1, v1), REG(r2, v2), REG(r3, v3), REG(r4, v4), REG(r5, v5), REG(r6, v6), REG(a6, F_LIB_STRUCT_AND_BASE))

#ifdef __MORPHOS__

#undef LIB_DEFFUNC0
#define LIB_DEFFUNC0(name, rt) \
        static rt name##_PPC(F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO0(name, rt) { rt rc; \
                rc = name##_PPC((APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC0NR
#define LIB_DEFFUNC0NR(name) \
        static void name##_PPC(F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO0NR(name) {name##_PPC((APTR)REG_A6);} \
        static void name##_PPC(F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC1
#define LIB_DEFFUNC1(name, rt, r1, t1, v1) \
        static rt name##_PPC(t1 v1, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO1(name, rt, r1, t1 v1) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC1NR
#define LIB_DEFFUNC1NR(name, r1, t1, v1) \
        static void name##_PPC(t1 v1, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO1NR(name, r1, t1 v1) { \
                name##_PPC((t1) REG_##r1, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC2
#define LIB_DEFFUNC2(name, rt, r1, t1, v1, r2, t2, v2) \
        static rt name##_PPC(t1 v1, t2 v2, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO2(name, rt, r1, t1 v1, r2, t2 v2) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (t2) REG_##r2, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, t2 v2, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC2NR
#define LIB_DEFFUNC2NR(name, r1, t1, v1, r2, t2, v2) \
        static void name##_PPC(t1 v1, t2 v2, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO2NR(name, r1, t1 v1, r2, t2 v2) { \
                name##_PPC((t1) REG_##r1, (t2) REG_##r2, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, t2 v2, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC3
#define LIB_DEFFUNC3(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3) \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO3(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC3NR
#define LIB_DEFFUNC3NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3) \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO3NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3) { \
                name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC4
#define LIB_DEFFUNC4(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO4(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC4NR
#define LIB_DEFFUNC4NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO4NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4) { \
                name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC5
#define LIB_DEFFUNC5(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5) \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO5(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (t5) REG_##r5, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC5NR
#define LIB_DEFFUNC5NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5) \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO5NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5) { \
                name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (t5) REG_##r5, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC6
#define LIB_DEFFUNC6(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5, r6, t6, v6) \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO6(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5, r6, t6 v6) { rt rc; \
                rc = name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (t5) REG_##r5, (t6) REG_##r6, (APTR)REG_A6); \
                REG_D0 = (uint32) rc; return rc; } \
        static rt name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, F_LIB_STRUCT_AND_BASE)

#undef LIB_DEFFUNC6NR
#define LIB_DEFFUNC6NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5, r6, t6, v6) \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, F_LIB_STRUCT_AND_BASE); \
        LIB_PROTO6NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5, r6, t6 v6) { \
                name##_PPC((t1) REG_##r1, (t2) REG_##r2, (t3) REG_##r3, (t4) REG_##r4, (t5) REG_##r5, (t6) REG_##r6, (APTR)REG_A6); } \
        static void name##_PPC(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, F_LIB_STRUCT_AND_BASE)

#else /* __MORPHOS__ */

#define LIB_DEFFUNC0(name, rt) LIB_PROTO0(name, rt)
#define LIB_DEFFUNC0NR(name) LIB_PROTO0NR(name)
#define LIB_DEFFUNC1(name, rt, r1, t1, v1) LIB_PROTO1(name, rt, r1, t1 v1)
#define LIB_DEFFUNC1NR(name, r1, t1, v1) LIB_PROTO1NR(name, r1, t1 v1)
#define LIB_DEFFUNC2(name, rt, r1, t1, v1, r2, t2, v2) LIB_PROTO2(name, rt, r1, t1 v1, r2, t2 v2)
#define LIB_DEFFUNC2NR(name, r1, t1, v1, r2, t2, v2) LIB_PROTO2NR(name, r1, t1 v1, r2, t2 v2)
#define LIB_DEFFUNC3(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3) LIB_PROTO3(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3)
#define LIB_DEFFUNC3NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3) LIB_PROTO3NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3)
#define LIB_DEFFUNC4(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) LIB_PROTO4(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4)
#define LIB_DEFFUNC4NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4) LIB_PROTO4NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4)
#define LIB_DEFFUNC5(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5) LIB_PROTO5(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5)
#define LIB_DEFFUNC5NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5) LIB_PROTO5NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5)
#define LIB_DEFFUNC6(name, rt, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5, r6, t6, v6) LIB_PROTO6(name, rt, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5, r6, t6 v6)
#define LIB_DEFFUNC6NR(name, r1, t1, v1, r2, t2, v2, r3, t3, v3, r4, t4, v4, r5, t5, v5, r6, t6, v6) LIB_PROTO6NR(name, r1, t1 v1, r2, t2 v2, r3, t3 v3, r4, t4 v4, r5, t5 v5, r6, t6 v6)

#endif /* __MORPHOS__ */
//+

///FC_Object

#ifdef __MORPHOS__

#define F_HOOK_PROTO(rt,name)                   _GATEPROTO(name); \
                                                extern rt (*name)( \
                                                    struct Hook * Hook, \
                                                    FObject Obj, \
                                                    uint32 * Msg) __attribute__((unused))

#define F_HOOK(rt,name)                         _DEFFUNC3(name, rt, \
                                                    a0, struct Hook *, Hook, \
                                                    a2, FObject, Obj, \
                                                    a1, uint32 *, Msg)

#define F_HOOKM_PROTO(rt,name,sn)               _GATEPROTO(name) ;\
                                                extern rt (*name)( \
                                                    struct Hook * Hook, \
                                                    FObject Obj, \
                                                    struct sn * Msg) __attribute__((unused))

#define F_HOOKM(rt,name,sn)                     _DEFFUNC3(name, rt, \
                                                    a0, struct Hook *, Hook, \
                                                    a2, FObject, Obj, \
                                                    a1, struct sn *, Msg)

#else /*** classic *********************************************************/
 
#define F_HOOK(rc,name)                         SAVEDS ASM(rc) name(REG(a0,struct Hook *Hook),REG(a2,FObject Obj),REG(a1,uint32 *Msg))
#define F_HOOKM(rc,name,sn)                     SAVEDS ASM(rc) name(REG(a0,struct Hook *Hook),REG(a2,FObject Obj),REG(a1,struct sn *Msg))
#define F_HOOK_PROTO(rc,name)                   F_HOOK(rc,name)
#define F_HOOKM_PROTO(rc,name,sn)               F_HOOKM(rc,name,sn)

#endif

//+
///FC_Class

#ifdef __MORPHOS__

/* Macro to be used to declare and define an object method */

#define F_METHOD_PROTO(rt,name)                 _GATEPROTO(name)
#define F_METHOD(rt,name)                       _LIB_DEFFUNC4(name, rt, \
                                                   a2, FClass *, Class, \
                                                   a0, FObject, Obj, \
                                                   d0, uint32, Method, \
                                                   a1, APTR, Msg)

/* Macro to be used to declare and define an object method, with a specific
type for Msg variable */

#define F_METHODM_PROTO(rt,name,sn)             _GATEPROTO(name)
#define F_METHODM(rt,name,sn)                   _LIB_DEFFUNC4(name, rt, \
                                                   a2, FClass *, Class, \
                                                   a0, FObject, Obj, \
                                                   d0, uint32, Method, \
                                                   a1, struct sn*, Msg)
                                                   
/* Macro to call a method (static in file or extern */

#define F_CALL_STATIC_METHOD(name, cl, obj, mth, msg, base) _PPC_##name(cl, obj, mth, msg, base)
#define F_CALL_EXTERN_METHOD(name, cl, obj, mth, msg, base) ({ \
                                                   REG_D0 = (uint32) (mth); \
                                                   REG_A0 = (uint32) (obj); \
                                                   REG_A1 = (uint32) (msg); \
                                                   REG_A2 = (uint32) (cl); \
                                                   REG_A6 = (uint32) (base); \
                                                   (*MyEmulHandle->EmulCallDirect68k)(name); })
 
#else /*** classic *********************************************************/

#define F_METHOD(rt,name)                       SAVEDS ASM(rt) name(REG(a2,FClass *Class),REG(a0,FObject Obj),REG(d0,uint32 Method),REG(a1,APTR Msg),REG(a6,struct FeelinBase *FeelinBase))
#define F_METHOD_PROTO(rt,name)                 F_METHOD(rt,name)
#define F_METHODM(rt,name,sn)                   SAVEDS ASM(rt) name(REG(a2,FClass *Class),REG(a0,FObject Obj),REG(d0,uint32 Method),REG(a1,struct sn *Msg),REG(a6,struct FeelinBase *FeelinBase))
#define F_METHODM_PROTO(rt,name,sn)             F_METHODM(rt,name,struct sn *)

#define F_CALL_EXTERN_METHOD(name, cl, obj, mth, msg, base) name(cl, obj, mth, msg, base)
#define F_CALL_STATIC_METHOD(name, cl, obj, mth, msg, base) F_CALL_EXTERN_METHOD(name, cl, obj, mth, msg, base)

#endif

//+
///FC_Thread
#ifdef __MORPHOS__

/* Macro to be used to define a thread entry function */

#define F_THREAD_ENTRY_PROTO(rt,name)           _GATEPROTO(name)
#define F_THREAD_ENTRY(rt,name)                 _LIB_DEFFUNC2NR(name, \
                                                   a0, FObject, Thread, \
                                                   a1, APTR, UserData)

#else /* classic */

#define F_THREAD_ENTRY_PROTO(name)              ASM(void) name(REG(a0,FObject Thread), REG(a1,APTR UserData), REG(a6,struct FeelinBase *FeelinBase))
#define F_THREAD_ENTRY(name)                    SAVEDS F_THREAD_ENTRY_PROTO(name)

#endif
//+

/*

GOFROMIEL: Ben comme tu vois j'ai déplacé ici les fonctions des  bases  des
bibliothèques. Elles sont utilisées par feelin.library, mais surtout toutes
les classes.

*/

///API basics
#ifdef __MORPHOS__

#define F_LIB_INIT                              struct Library * lib_init(void)
#define F_LIB_EXPUNGE                           uint32 lib_expunge(void)
#define F_LIB_CLOSE                             void lib_close(void)
#define F_LIB_OPEN                              struct Library * lib_open(void)
#define F_LIB_QUERY                             struct TagItem * lib_query(void)

/* args */

#define F_LIB_INIT_ARGS                         F_LIB_STRUCT_AND_BASE = (APTR) REG_D0; \
                                                uint32 SegList = (uint32) REG_A0; \
                                                struct ExecBase *SYS = (struct ExecBase *) REG_A6;
#define F_LIB_EXPUNGE_ARGS                      F_LIB_STRUCT_AND_BASE = (APTR) REG_A6;
#define F_LIB_CLOSE_ARGS                        F_LIB_STRUCT_AND_BASE = (APTR) REG_A6;
#define F_LIB_OPEN_ARGS                         F_LIB_STRUCT_AND_BASE = (APTR) REG_A6;
#define F_LIB_QUERY_ARGS                        uint32 Which = (uint32) REG_D0; \
                                                struct FeelinBase *Feelin = (struct FeelinBase *) REG_A0; \
                                                F_LIB_STRUCT_AND_BASE = (APTR) REG_A6;

#else /*** classic *********************************************************/

#define F_LIB_INIT                              ASM(struct Library *) lib_init \
                                                ( \
                                                   REG(d0,F_LIB_STRUCT_AND_BASE), \
                                                   REG(a0,uint32 SegList), \
                                                   REG(a6,struct ExecBase *SYS) \
                                                )

#define F_LIB_EXPUNGE                           ASM(uint32) lib_expunge \
                                                ( \
                                                   REG(a6,F_LIB_STRUCT_AND_BASE) \
                                                )

#define F_LIB_CLOSE                             ASM(uint32) lib_close \
                                                ( \
                                                   REG(a6,F_LIB_STRUCT_AND_BASE) \
                                                )

#define F_LIB_OPEN                              ASM(struct Library *) lib_open \
                                                ( \
						   REG(d0, ULONG Version), \
                                                   REG(a6,F_LIB_STRUCT_AND_BASE) \
                                                )

#define F_LIB_QUERY                             ASM(struct TagItem *) lib_query \
                                                ( \
                                                    REG(d0,uint32 Which), \
                                                    REG(a0,struct FeelinBase *Feelin), \
                                                    REG(a6,F_LIB_STRUCT_AND_BASE) \
                                                )

#define F_LIB_INIT_ARGS
#define F_LIB_EXPUNGE_ARGS
#define F_LIB_CLOSE_ARGS
#define F_LIB_OPEN_ARGS
#define F_LIB_QUERY_ARGS

#endif
//+

#endif
