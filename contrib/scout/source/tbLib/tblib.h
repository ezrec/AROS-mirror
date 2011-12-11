#ifndef TBLIB_H
#define TBLIB_H 1

#ifndef EXEC_TYPES_H
    #include <exec/types.h>
#endif
#ifndef EXEC_PORTS_H
    #include <exec/ports.h>
#endif
#ifndef DOS_DOS_H
    #include <dos/dos.h>
#endif
#ifndef DEVICES_TIMER_H
    #include <devices/timer.h>
#endif
#ifndef INTUITION_CLASSES_H
    #include <intuition/classes.h>
#endif
#ifndef INTUITION_CLASSUSR_H
    #include <intuition/classusr.h>
#endif
#ifndef UTILITY_TAGITEM_H
    #include <utility/tagitem.h>
#endif
#ifndef _STDARG_H
    #include <stdarg.h>
#endif
#ifndef SDI_COMPILER_H
    #include <SDI/SDI_compiler.h>
#endif
#ifndef SDI_STDARG_H
    #include <SDI/SDI_stdarg.h>
#endif
#ifndef SDI_HOOK_H
    #include <SDI/SDI_hook.h>
#endif

#if !defined(__amigaos4__) || !defined(__NEW_TIMEVAL_DEFINITION_USED__)
struct TimeVal
{
  ULONG Seconds;
  ULONG Microseconds;
};

struct TimeRequest
{
  struct IORequest Request;
  struct TimeVal   Time;
};
#endif

/* useful functions for MsgPorts and Signals */
BOOL AllocPort( struct MsgPort *port );
void FreePort( struct MsgPort *port );
BYTE AllocSig( BYTE instead );
void FreeSig ( BYTE sig );
void SafePutMsg( STRPTR portname, struct Message *msg, BOOL wait4reply );

/* useful functions for timerequests */
void TimerDelay( struct TimeRequest *treq, ULONG secs, ULONG mics );
void StartTimer( struct TimeRequest *treq, ULONG secs, ULONG mics );
#define StopTimer(treq) GetMsg((treq)->Request.io_Message.mn_ReplyPort)
void AbortTimer( struct TimeRequest *treq );

/* little-endian <-> big-endian conversions */
UWORD ASM i2m_word( REG(d0, UWORD d) );
ULONG ASM i2m_long( REG(d0, ULONG d) );

/* 64bit math functions */
struct Integer64 {
    ULONG i64_Upper;
    ULONG i64_Lower;
};

#if defined(__GNUC__)
/* dst += src */
void add64 ( struct Integer64 *dst, struct Integer64 *src );
/* dst -= src */
void sub64 ( struct Integer64 *dst, struct Integer64 *src );
/*
    dst < src       -1
    dst = src        0
    dst > src        1
*/
int cmp64 ( struct Integer64 *dst, struct Integer64 *src );
#else
/* dst += src */
void ASM add64 ( REG(a0, struct Integer64 *dst), REG(a1, struct Integer64 *src) );
/* dst -= src */
void ASM sub64 ( REG(a0, struct Integer64 *dst), REG(a1, struct Integer64 *src) );
/*
    dst < src       -1
    dst = src        0
    dst > src        1
*/
int ASM cmp64 ( REG(a0, struct Integer64 *dst), REG(a1, struct Integer64 *src) );
#endif

/* useful functions for strings */
void _vsprintf( STRPTR buffer, CONST_STRPTR fmt, VA_LIST args );
BOOL _vsnprintf( STRPTR buffer, LONG maxlen, CONST_STRPTR fmt, VA_LIST args );
void VARARGS68K STDARGS _sprintf( STRPTR buffer, CONST_STRPTR fmt,... );
BOOL VARARGS68K STDARGS _snprintf( STRPTR buffer, LONG maxlen, CONST_STRPTR fmt,... );

STRPTR _strdup( CONST_STRPTR str );
STRPTR _strdup_pool( CONST_STRPTR str, APTR pool );
void _strcat( STRPTR *str, CONST_STRPTR cat );
void _strcatn( STRPTR str, CONST_STRPTR cat, ULONG maxlen );
void b2cstr( BSTR b, STRPTR c );
void b2cstrn( BSTR b, STRPTR c, ULONG maxlen );
void c2bstr( STRPTR c, BSTR b );
STRPTR _strsep( STRPTR *str, STRPTR delim );
ULONG parse_string( STRPTR str, STRPTR delim, STRPTR *argv, ULONG max_arg );

void VARARGS68K STDARGS ErrorMsg( CONST_STRPTR title, CONST_STRPTR msg, ... );

/* function to check version of a library/device */
ULONG CheckLibVersion( struct Library *lib, UWORD version, UWORD revision );
ULONG CheckLibVersionName( CONST_STRPTR lib, UWORD version, UWORD revision );

#define CLV_NOT_OPEN        0
#define CLV_OLDER           1
#define CLV_NEWER_OR_SAME   2

/* function to get/set environment variables */
LONG GetEnvVar( STRPTR name, STRPTR buffer, ULONG size, ULONG flags );
BOOL SetEnvVar( STRPTR name, STRPTR buffer, ULONG size, ULONG flags, BOOL save );
#define SetEnvVarSimple(name, buffer, save) SetEnvVar((name), (buffer), strlen((buffer)), GVF_GLOBAL_ONLY, (save))

/* function for pooled memory allocations */
#if defined(__GNUC__)
#define tbCreatePool( flags, puddleSize, threshSize)        CreatePool(flags, puddleSize, threshSize)
#define tbAllocPooled( poolHeader, memSize)                 AllocPooled(poolHeader, memSize)
#define tbAllocVecPooled( poolHeader, memSize)              AllocVecPooled(poolHeader, memSize)
#define tbFreePooled( poolHeader, memory, memSize)          FreePooled(poolHeader, memory, memSize)
#define tbFreeVecPooled( poolHeader, memory)                FreeVecPooled(poolHeader, memory)
#define tbDeletePool( poolHeader)                           DeletePool(poolHeader)

#ifndef AllocVecPooled
APTR AllocVecPooled( APTR poolHeader, ULONG memSize );
#endif
#ifndef FreeVecPooled
void FreeVecPooled( APTR poolHeader, APTR memory );
#endif

#else
void * ASM tbCreatePool( REG(d0, ULONG flags), REG(d1, ULONG puddleSize), REG(d2, ULONG threshSize) );
APTR ASM tbAllocPooled( REG(a0, APTR poolHeader), REG(d0, ULONG memSize) );
APTR ASM tbAllocVecPooled( REG(a0, APTR poolHeader), REG(d0, ULONG memSize) );
void ASM tbFreePooled( REG(a0, APTR poolHeader), REG(a1, APTR memory), REG(d0, ULONG memSize) );
void ASM tbFreeVecPooled( REG(a0, APTR poolHeader), REG(a1, APTR memory) );
void ASM tbDeletePool( REG(a0, void *poolHeader) );
#endif

#ifndef __MORPHOS__
#if defined(__AROS__)
IPTR DoSuperNew(struct IClass *cl, Object *obj, Tag tag1, ...);
#else
ULONG VARARGS68K STDARGS DoSuperNew( struct IClass *cl, Object *obj, ... );
#endif
#endif

APTR MakeButton( CONST_STRPTR txt );
APTR MakeCheckmark( CONST_STRPTR txt );

#if defined(__AROS__)
#define xget XGET
#else
#if defined(__GNUC__)
#define xget(obj, attr) ({ULONG b = 0; GetAttr(attr, obj, (APTR)&b); b;})
#else
ULONG xget( Object *obj, ULONG attr );
#endif
#endif

#define FLAG_IS_SET(mask, flag)         (((mask) & (flag)) == (flag))
#define FLAG_IS_CLEAR(mask, flag)       (((mask) & (flag)) == 0)
#define BIT_IS_SET(mask, bit)           FLAG_IS_SET(mask, 1 << bit)
#define BIT_IS_CLEAR(mask, bit)         FLAG_IS_CLEAR(mask, 1 << bit)

#define SET_FLAG(mask, flag)            (mask) |= (flag)
#define CLEAR_FLAG(mask, flag)          (mask) &= ~(flag)
#define SET_BIT(mask, bit)              SET_FLAG(mask, 1 << bit)
#define CLEAR_BIT(mask, bit)            CLEAR_FLAG(mask, 1 << bit)

#define ARRAY_SIZE(x)                   (sizeof(x) / sizeof(x[0]))

#if defined(__amigaos4__)
#define GETINTERFACE(iface, base)	(iface = (APTR)GetInterface((struct Library *)(base), "main", 1L, NULL))
#define DROPINTERFACE(iface)			(DropInterface((struct Interface *)iface), iface = NULL)
#else
#define GETINTERFACE(iface, base)	TRUE
#define DROPINTERFACE(iface)
#endif

#endif

