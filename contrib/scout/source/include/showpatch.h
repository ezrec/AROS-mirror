#ifndef SHOWPATCH_H
#define SHOWPATCH_H 1

/*************************************************
 ** ShowPatch                                   **
 **                                             **
 ** Zeige die Patches und die Patcher von       **
 ** Saferpatches.                               **
 **     © 1994,97,99,2001 THOR-Software inc.    **
 **     Version 2.23 vom 11.10.2001             **
 *************************************************/

#include "amiga-align.h"

/*************************************************
 ** PatchPort                                   **
 **                                             **
 ** Supervisor Port                             **
 *************************************************/
struct PatchPort {
        struct MsgPort          pp_Port;                /* embedded port structure used for identification */
        UWORD                   pp_Flags;               /* flags, see below */
        APTR                    pp_RemoveProc;          /* not for public use */
        APTR                    pp_OldSetFunction;      /* not for public use */
        struct MinList          pp_PatchList;           /* embedded patch list */
        struct SignalSemaphore  pp_Semaphore;           /* semaphore, not used */
};

#define PPF_REMEMBER    (1<<0L)

#define PATCHPORT_NAME  "SaferPatches.rendezvous"

/*************************************************
 ** LibPatchList                                **
 *************************************************/
struct LibPatchList {
        struct LibPatchList     *ll_next,*ll_pred;      /* node fields */
        void                    *ll_Root;               /* not used */
        struct Library          *ll_LibBase;            /* library of this node */
        struct MinList           ll_PatchList;          /* all patch nodes of this library */
};

/*************************************************
 ** LibPatchNode                                **
 *************************************************/
struct LibPatchNode {
        struct LibPatchNode     *ln_next,*ln_pred;      /* node fields */
        struct LibPatchList     *ln_Root;               /* thread field */
        WORD                     ln_Offset;             /* patched offset */
        WORD                     ln_reserved;           /* not used */
        struct MinList           ln_PatchList;          /* all patch entries of this offset */
};

/*************************************************
 ** LibPatchEntry                               **
 *************************************************/
struct LibPatchEntry {
        struct LibPatchEntry    *le_next,*le_pred;      /* node fields */
        struct LibPatchNode     *le_Root;               /* thread field */
        WORD                     le_Flags;              /* Flags, see below */
        UWORD                    le_Jmp;                /* 0x4ef9: JMP-instructions */
        void                    (*le_OldEntry)();       /* pointer to old function */
        void                    (*le_NewEntry)();       /* new entry */
        char                    *le_Patcher;            /* name of patching process */
        void                    (*le_JumpBack)();       /* really old pointer */

};

#define LEF_REMOVED     (1<<1L)
#define LEF_DISABLED    (1<<2L)
#define LEF_SELECTED    (1<<8L)

#include "default-align.h"

#endif /* SHOWPATCH_H */

