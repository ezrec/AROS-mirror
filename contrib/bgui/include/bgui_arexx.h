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
 * Revision 42.11  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.10  2003/01/18 19:10:21  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.9  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.8  2000/08/08 20:24:51  stegerg
 * had to remove the "compilerspecific.h" #include and instead insert
 * the REGPARAM/etc. macros also in this file. This is because compilerspecific.h
 * can be used only by the stuff coming in the BGUI source package.
 * Apps like the BGUI version of the calculator cannot use it.
 *
 * Revision 42.7  2000/08/08 14:02:08  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.6  2000/08/07 21:50:30  stegerg
 * fixed/activated REGFUNC/REGPARAM macros.
 *
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
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
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

#ifdef __AROS__
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

#endif /* __AROS__ */

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
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

#ifndef __AROS__
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
**      An array of these structures must be passed at object-create time.
**/
#ifdef __AROS__
//#warning Deactivated the following typedef
#else
typedef struct {
        UBYTE                   *rc_Name;         /* Command name. */
        UBYTE                   *rc_ArgTemplate;  /* DOS-style argument template. */
//        ASM VOID                (*rc_Func)( REG(a0) REXXARGS *, REG(a1) struct RexxMsg * );
        ASM REGFUNCPROTO2(VOID, (*rc_Func),
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
