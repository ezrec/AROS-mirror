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

#ifndef REXX_STORAGE_H
#include <rexx/storage.h>
#endif

#ifndef REXX_RXSLIB_H
#include <rexx/rxslib.h>
#endif

#ifndef REXX_ERRORS_H
#include <rexx/errors.h>
#endif

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

struct acmExecute {
        ULONG                   MethodID;
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
#ifndef ASM
#define ASM       __asm
#endif
#ifndef REG
#define REG(x)    register __ ## x
#endif
#endif

/*
**      An array of these structures must be passed at object-create time.
**/
typedef struct {
        UBYTE                   *rc_Name;         /* Command name. */
        UBYTE                   *rc_ArgTemplate;  /* DOS-style argument template. */
        ASM VOID                (*rc_Func)( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
}       REXXCOMMAND;

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
