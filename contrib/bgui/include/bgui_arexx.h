#ifndef BGUI_AREXX_H
#define BGUI_AREXX_H
/*
 * @(#) $Header$
 *
 * $VER: bgui_arexx.h 41.10 (11.10.98)
 * C Header for the BOOPSI ARexx interface class.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.5  2000/07/07 17:15:54  stegerg
 * stack??? stuff in method structs.
 *
 * Revision 42.4  2000/07/03 20:58:42  bergers
 * Automatically installs library and test program in AROS work directory.
 *
 * Revision 42.3  2000/07/02 04:32:45  bergers
 * Removed a noisy warning.
 *
 * Revision 42.2  2000/05/29 00:40:25  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define _AROS. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/15 19:28:20  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:23:05  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:37:09  mlemos
 * Bumped to revision 41.11
 *
 * Revision 41.1  2000/05/09 20:01:41  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/19 05:00:55  mlemos
 * Added support for Storm C.
 *
 * Revision 1.1.2.1  1998/10/12 01:47:18  mlemos
 * Initial revision integrated from Ian sources.
 *
 *
 *
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef DOS_RDARGS_H
#include <dos/rdargs.h>
#endif

#ifdef _AROS
// Not including any AREXX includes
#else

#ifndef REXX_STORAGE_H
#include <rexx/storage.h>
#endif

#ifndef REXX_RXSLIB_H
#include <rexx/rxslib.h>
#endif

#ifndef REXX_ERRORS_H
#include <rexx/errors.h>
#endif

#endif /* _AROS */

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

/* Tags */
#define AREXX_TB                        (TAG_USER+0x30000)

#define AREXX_HostName                  (AREXX_TB+1)    /* I-G-- */
#define AREXX_FileExtension             (AREXX_TB+2)    /* I---- */
#define AREXX_CommandList               (AREXX_TB+3)    /* I---- */
#define AREXX_SignalMask                (AREXX_TB+4)    /* --G-- */
#define AREXX_ErrorCode                 (AREXX_TB+5)    /* I---- */

/* Methods */
#define AREXX_MB                        (0x30000)

/* ARexx class event-handler. */
#define AREXXM_HANDLE_EVENT             (AREXX_MB+1)

/* Execute a host command. */
#define AREXXM_EXECUTE                  (AREXX_MB+2)

#ifndef _AROS
#undef STACKULONG
#define STACKULONG ULONG
#endif

struct acmExecute {
        STACKULONG                   MethodID;
        UBYTE                  *acme_CommandString;
        LONG                   *acme_RC;
        LONG                   *acme_RC2;
        UBYTE                 **acme_Result;
        BPTR                    acme_IO;
};

/*
**      The routines from the command-list will receive a pointer
**      to this structure.  In this structure are the parsed arguments
**      and storage to put the results of the command.
**/
typedef struct {
        ULONG                   *ra_ArgList;      /* Result of ReadArgs(). */
        LONG                     ra_RC;           /* Primary result. */
        LONG                     ra_RC2;          /* Secondary result. */
        UBYTE                   *ra_Result;       /* RESULT variable. */
}       REXXARGS;

/*
 * Compiler specific stuff.
 */
#ifdef _DCC
#ifndef ASM
#define ASM
#endif
#ifndef REG
#define REG(x)    __ ## x
#endif
#elif __STORM__
#ifndef SAVEDS
#define SAVEDS    __saveds
#endif
#ifndef ASM
#define ASM
#endif
#ifndef REG
#define REG(x)    register __ ## x
#endif
#else
#ifndef _AROS
  #ifndef ASM
  #define ASM       __asm
  #endif
  #ifndef REG
  #define REG(x)    register __ ## x
  #endif
  #endif
#endif

#ifdef _AROS

  #ifndef AROS_ASMCALL_H
  #include <aros/asmcall.h>
  #endif
  
  #ifdef ASM
  #undef ASM
  #endif
  #define ASM
  
  #ifdef SAVEDS
  #undef SAVEDS
  #endif
  #define SAVEDS

#if 0
  #ifndef REGPARAM
  #define REGPARAM(reg,type,name) AROS_UFHA(type, name, reg)
  
  #define REGFUNC1(r,n,a1) AROS_UFH1(r,n,a1)
  #define REGFUNC2(r,n,a1,a2) AROS_UFH2(r,n,a1,a2)
  #define REGFUNC3(r,n,a1,a2,a3) AROS_UFH3(r,n,a1,a2,a3)
  #define REGFUNC4(r,n,a1,a2,a3,a4) AROS_UFH4(r,n,a1,a2,a3,a4)
  #define REGFUNC5(r,n,a1,a2,a3,a4,a5) AROS_UFH5(r,n,a1,a2,a3,a4,a5)
  #define REGFUNC6(r,n,a1,a2,a3,a4,a5,a6) AROS_UFH6(r,n,a1,a2,a3,a4,a5,a6)
  #define REGFUNC7(r,n,a1,a2,a3,a4,a5,a6,a7) AROS_UFH7(r,n,a1,a2,a3,a4,a5,a6,a7)
  #define REGFUNC8(r,n,a1,a2,a3,a4,a5,a6,a7,a8) AROS_UFH8(r,n,a1,a2,a3,a4,a5,a6,a7,a8)
  #define REGFUNC9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9) AROS_UFH9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9)
  #endif
#else
  #ifndef REGPARAM
  #define REGPARAM(reg,type,name) type name
  
  #define REGFUNC1(r,n,a1) r n(a1)
  #define REGFUNC2(r,n,a1,a2) r n(a1,a2)
  #define REGFUNC3(r,n,a1,a2,a3) r n(a1,a2,a3)
  #define REGFUNC4(r,n,a1,a2,a3,a4) r n(a1,a2,a3,a4)
  #define REGFUNC5(r,n,a1,a2,a3,a4,a5) r n(a1,a2,a3,a4,a5)
  #define REGFUNC6(r,n,a1,a2,a3,a4,a5,a6) r n(a1,a2,a3,a4,a5,a6)
  #define REGFUNC7(r,n,a1,a2,a3,a4,a5,a6,a7) r n(a1,a2,a3,a4,a5,a6,a7)
  #define REGFUNC8(r,n,a1,a2,a3,a4,a5,a6,a7,a8) r n(a1,a2,a3,a4,a5,a6,a7,a8)
  #define REGFUNC9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9) r n(a1,a2,a3,a4,a5,a6,a7,a8,a9)
  #endif
#endif
  
#else
  #ifndef REGPARAM
  #define REGPARAM(reg,type,name) REG(reg) type name
  
  #define REGFUNC1(r,n,a1) r n(a1)
  #define REGFUNC2(r,n,a1,a2) r n(a1,a2)
  #define REGFUNC3(r,n,a1,a2,a3) r n(a1,a2,a3)
  #define REGFUNC4(r,n,a1,a2,a3,a4) r n(a1,a2,a3,a4)
  #define REGFUNC5(r,n,a1,a2,a3,a4,a5) r n(a1,a2,a3,a4,a5)
  #define REGFUNC6(r,n,a1,a2,a3,a4,a5,a6) r n(a1,a2,a3,a4,a5,a6)
  #define REGFUNC7(r,n,a1,a2,a3,a4,a5,a6,a7) r n(a1,a2,a3,a4,a5,a6,a7)
  #define REGFUNC8(r,n,a1,a2,a3,a4,a5,a6,a7,a8) r n(a1,a2,a3,a4,a5,a6,a7,a8)
  #define REGFUNC9(r,n,a1,a2,a3,a4,a5,a6,a7,a8,a9) r n(a1,a2,a3,a4,a5,a6,a7,a8,a9)
  #endif
  
#endif
/*
**      An array of these structures must be passed at object-create time.
**/
#ifdef _AROS
//#warning Deactivated the following typedef
#else
typedef struct {
        UBYTE                   *rc_Name;         /* Command name. */
        UBYTE                   *rc_ArgTemplate;  /* DOS-style argument template. */
//        ASM VOID                (*rc_Func)( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
        ASM REGFUNC2(VOID, (*rc_Func),
		REGPARAM(A0, REXXARGS *,),
		REGPARAM(A1, struct RexxMsg *, ));
}       REXXCOMMAND;
#endif
/*
**      Possible errors.
**/
#define RXERR_NO_COMMAND_LIST           (1L)
#define RXERR_NO_PORT_NAME              (2L)
#define RXERR_PORT_ALREADY_EXISTS       (3L)
#define RXERR_OUT_OF_MEMORY             (4L)

/*
 * Backwards compatibility.
 */
#define AC_HostName                     AREXX_HostName
#define AC_FileExtention                AREXX_FileExtension
#define AC_CommandList                  AREXX_CommandList
#define AC_ErrorCode                    AREXX_ErrorCode
#define AC_RexxPortMask                 AREXX_SignalMask
#define ACM_HANDLE_EVENT                AREXXM_HANDLE_EVENT
#define ACM_EXECUTE                     AREXXM_EXECUTE

#endif
