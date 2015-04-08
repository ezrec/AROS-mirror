/*************************************************************************\
 *                                                                       *
 * config.h                                                              *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <exec/types.h>
#ifdef MORPHOS
#   define EMUL_NOQUICKMODE 1
#   include <emul/emulinterface.h>
#   include <emul/emulregs.h>
#endif

#define NO_INLINE_STDARG
#define NO_PPCINLINE_STDARG

#ifdef __SASC
#   define SAVEDS   __saveds
#   define ASM      __asm
#   define REG(r,a) register __ ## r a
#elif defined(__GNUC__)
#   define SAVEDS
#   define ASM
#   define REG(r,a) a __asm(#r)
#else
#   error Customize for your compiler...
#endif

#ifdef MORPHOS

#   define BEGIN_HOOK(R,name,T1,p1,T2,p2,T3,p3) \
    R name##_func() {                           \
	T1 p1=(T1)REG_A0;                       \
	T2 p2=(T2)REG_A2;                       \
	T3 p3=(T3)REG_A1;
#   define END_HOOK(name)                   \
    }                                       \
    struct EmulLibEntry name##_entry={      \
	TRAP_LIB,0,(void(*)())name##_func   \
    };                                      \
    struct Hook name##_hook={{NULL,NULL},(ULONG(*)())&name##_entry};

#   define BEGIN_STATIC_HOOK(R,name,T1,p1,T2,p2,T3,p3) \
    static R name##_func() {                    \
	T1 p1=(T1)REG_A0;                       \
	T2 p2=(T2)REG_A2;                       \
	T3 p3=(T3)REG_A1;
#   define END_STATIC_HOOK(name)              \
    }                                         \
    static struct EmulLibEntry name##_entry={ \
	TRAP_LIB,0,(void(*)())name##_func     \
    };                                        \
    static struct Hook name##_hook={{NULL,NULL},(ULONG(*)())&name##_entry};

#   define BEGIN_DISPATCHER(name,p1,p2,p3)         \
    static ULONG name##_entry() {                  \
	struct IClass *p1=(struct IClass *)REG_A0; \
	Object *p2=(Object*)REG_A2;                \
	Msg p3=(Msg)REG_A1;
#   define END_DISPATCHER(name)             \
    }                                       \
    static struct EmulLibEntry name={       \
	TRAP_LIB,0,(void(*)())name##_entry  \
    };

ULONG myDoMethod(void *, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);

#define DoMethod2(o,a1) ((ULONG(*)(void *,ULONG))myDoMethod)(o,(ULONG)(a1))
#define DoMethod3(o,a1,a2) ((ULONG(*)(void *,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2))
#define DoMethod4(o,a1,a2,a3) ((ULONG(*)(void *,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3))
#define DoMethod5(o,a1,a2,a3,a4) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4))
#define DoMethod6(o,a1,a2,a3,a4,a5) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5))
#define DoMethod7(o,a1,a2,a3,a4,a5,a6) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6))
#define DoMethod8(o,a1,a2,a3,a4,a5,a6,a7) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6),(ULONG)(a7))
#define DoMethod9(o,a1,a2,a3,a4,a5,a6,a7,a8) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6),(ULONG)(a7),(ULONG)(a8))
#define DoMethod10(o,a1,a2,a3,a4,a5,a6,a7,a8,a9) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6),(ULONG)(a7),(ULONG)(a8),(ULONG)(a9))
#define DoMethod11(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6),(ULONG)(a7),(ULONG)(a8),(ULONG)(a9),(ULONG)(a10))
#define DoMethod12(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) ((ULONG(*)(void *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))myDoMethod)(o,(ULONG)(a1),(ULONG)(a2),(ULONG)(a3),(ULONG)(a4),(ULONG)(a5),(ULONG)(a6),(ULONG)(a7),(ULONG)(a8),(ULONG)(a9),(ULONG)(a10),(ULONG)(a11))

/*#define DoMethod(obj, tags...) \
	({ULONG _tags[] = { tags }; DoMethodA((obj), (APTR)_tags);})*/
/*#define CoerceMethod(cl, obj, tags...) \
	({ULONG _tags[] = { tags }; CoerceMethodA((cl), (obj), (APTR)_tags);})*/
#define CoerceMethod(cl, obj, m) ({ ULONG t=m; CoerceMethodA(cl,obj,&t); })

#else /* MORPHOS */

#   define BEGIN_HOOK(R,name,T1,p1,T2,p2,T3,p3) \
    R SAVEDS ASM name##_func(REG(a0,T1 p1),REG(a2,T2 p2),REG(a1,T3 p3))
#   define END_HOOK(name) \
    struct Hook name##_hook={{NULL,NULL},(ULONG(*)())name##_func};

#   define BEGIN_STATIC_HOOK(R,name,T1,p1,T2,p2,T3,p3) \
    static R SAVEDS ASM name##_func(REG(a0,T1 p1),REG(a2,T2 p2),REG(a1,T3 p3))
#   define END_STATIC_HOOK(name) \
    static struct Hook name##_hook={{NULL,NULL},(ULONG(*)())name##_func};

#   define BEGIN_DISPATCHER(name,p1,p2,p3) \
    static ULONG SAVEDS ASM name(REG(a0,struct IClass *p1),REG(a2,Object *p2),REG(a1,Msg p3))
#   define END_DISPATCHER(name)

#define DoMethod2(o,a1) DoMethod(o,a1)
#define DoMethod3(o,a1,a2) DoMethod(o,a1,a2)
#define DoMethod4(o,a1,a2,a3) DoMethod(o,a1,a2,a3)
#define DoMethod5(o,a1,a2,a3,a4) DoMethod(o,a1,a2,a3,a4)
#define DoMethod6(o,a1,a2,a3,a4,a5) DoMethod(o,a1,a2,a3,a4,a5)
#define DoMethod7(o,a1,a2,a3,a4,a5,a6) DoMethod(o,a1,a2,a3,a4,a5,a6)
#define DoMethod8(o,a1,a2,a3,a4,a5,a6,a7) DoMethod(o,a1,a2,a3,a4,a5,a6,a7)
#define DoMethod9(o,a1,a2,a3,a4,a5,a6,a7,a8) DoMethod(o,a1,a2,a3,a4,a5,a6,a7,a8)
#define DoMethod10(o,a1,a2,a3,a4,a5,a6,a7,a8,a9) DoMethod(o,a1,a2,a3,a4,a5,a6,a7,a8,a9)
#define DoMethod11(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) DoMethod(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)
#define DoMethod12(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) DoMethod(o,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)

#endif /* MORPHOS */

#endif

