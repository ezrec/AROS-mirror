/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef _COMPILER_H
#define _COMPILER_H 1

/*------------------------------------------------------------------------*/

#ifdef __MORPHOS__

#include <emul/emulinterface.h>
#include <emul/emulregs.h>

#define MCC_DISPATCHER(name) \
	ULONG name##_dispatcher(void); \
	struct EmulLibEntry GATE##name##_dispatcher = { TRAP_LIB, 0, (void (*)(void))name##_dispatcher }; \
	ULONG name##_dispatcher(void) { struct IClass *cl=(struct IClass*)REG_A0; Msg msg=(Msg)REG_A1; Object *obj=(Object*)REG_A2;

#define MCC_DISPATCHER_END }
#define MCC_DISPATCHER_REF(name) &GATE##name##_dispatcher
#define MCC_DISPATCHER_EXTERN(name) extern ULONG name##_dispatcher(void); extern struct EmulLibEntry GATE##name##_dispatcher

#define A0(type,name) type name = (type)REG_A0
#define A1(type,name) type name = (type)REG_A1
#define A2(type,name) type name = (type)REG_A2
#define A3(type,name) type name = (type)REG_A3
#define A4(type,name) type name = (type)REG_A4
#define A5(type,name) type name = (type)REG_A5
#define A6(type,name) type name = (type)REG_A6
#define A7(type,name) type name = (type)REG_A7
#define D0(type,name) type name = (type)REG_D0
#define D1(type,name) type name = (type)REG_D1
#define D2(type,name) type name = (type)REG_D2
#define D3(type,name) type name = (type)REG_D3
#define D4(type,name) type name = (type)REG_D4
#define D5(type,name) type name = (type)REG_D5
#define D6(type,name) type name = (type)REG_D6
#define D7(type,name) type name = (type)REG_D7

#define MAKEHOOK(hookname, funcname) \
	struct EmulLibEntry GATE_##funcname##_hook = { TRAP_LIB, 0, (void (*)(void))funcname }; \
	struct Hook hookname = {{NULL,NULL}, (HOOKFUNC)&GATE_##funcname##_hook, NULL, NULL}

#define MAKESTATICHOOK(hookname, funcname) \
	static struct EmulLibEntry GATE_##funcname##_hook = { TRAP_LIB, 0, (void (*)(void))funcname }; \
	static struct Hook hookname = {{NULL,NULL}, (HOOKFUNC)&GATE_##funcname##_hook, NULL, NULL}

#elif __AROS__

#include <SDI/SDI_hook.h>

#else /* OS3 */

#define MCC_DISPATCHER(name) \
	ULONG name##_dispatcher(void) { \
		register struct IClass *a0 __asm("a0"); struct IClass  *cl = a0; \
		register Msg				a1 __asm("a1"); Msg				msg = a1; \
		register Object		  *a2 __asm("a2"); Object		  *obj = a2;

#define MCC_DISPATCHER_END }
#define MCC_DISPATCHER_REF(name) name##_dispatcher
#define MCC_DISPATCHER_EXTERN(name) extern ULONG name##_dispatcher(void);

#define A0(type,name) register type a0 __asm("a0"); type name = a0
#define A1(type,name) register type a1 __asm("a1"); type name = a1
#define A2(type,name) register type a2 __asm("a2"); type name = a2
#define A3(type,name) register type a3 __asm("a3"); type name = a3
#define A4(type,name) register type a4 __asm("a4"); type name = a4
#define A5(type,name) register type a5 __asm("a5"); type name = a5
#define A6(type,name) register type a6 __asm("a6"); type name = a6
#define A7(type,name) register type a7 __asm("a7"); type name = a7
#define D0(type,name) register type d0 __asm("d0"); type name = d0
#define D1(type,name) register type d1 __asm("d1"); type name = d1
#define D2(type,name) register type d2 __asm("d2"); type name = d2
#define D3(type,name) register type d3 __asm("d3"); type name = d3
#define D4(type,name) register type d4 __asm("d4"); type name = d4
#define D5(type,name) register type d5 __asm("d5"); type name = d5
#define D6(type,name) register type d6 __asm("d6"); type name = d6
#define D7(type,name) register type d7 __asm("d7"); type name = d7

#define MAKEHOOK(hookname, funcname) \
	struct Hook hookname = {{NULL,NULL}, (HOOKFUNC)funcname, NULL, NULL}

#define MAKESTATICHOOK(hookname, funcname) \
	static struct Hook hookname = {{NULL,NULL}, (HOOKFUNC)funcname, NULL, NULL}

#endif

/*------------------------------------------------------------------------*/

#endif

