/*
 * @(#) $Header$
 *
 * $VER: clib/bgui_obsolete.h 41.10 (25.2.98)
 * bgui.library obsolete definitions
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:23:21  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:55  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1999/08/29 20:25:51  mlemos
 * Moved the definitions for methods GRM_ADDMEMBER, GRM_REMMEMBER,
 * GRM_DIMENSIONS, GRM_ADDSPACEMEMBER, GRM_INSERTMEMBER, GRM_REPLACEMEMBER to
 * bgui.h.
 *
 * Revision 41.10  1998/02/25 21:13:56  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:16:03  mlemos
 * Ian sources
 *
 *
 */




/* New methods */
#define BASE_ADDMAP                     (BGUI_MB+41)

/* Add an object to the maplist notification list. */
struct bmAddMap {
        ULONG                   MethodID;
        Object                 *bam_Object;
        struct TagItem         *bam_MapList;
};

#define BASE_ADDCONDITIONAL             (BGUI_MB+42)

/* Add an object to the conditional notification list. */
struct bmAddConditional {
        ULONG                   MethodID;
        Object                 *bac_Object;
        struct TagItem          bac_Condition;
        struct TagItem          bac_TRUE;
        struct TagItem          bac_FALSE;
};

#define BASE_ADDMETHOD                  (BGUI_MB+43)

/* Add an object to the method notification list. */
struct bmAddMethod {
        ULONG                   MethodID;
        Object                 *bam_Object;
        ULONG                   bam_Flags;
        ULONG                   bam_Size;
        ULONG                   bam_MethodID;
};

#define BAMF_NO_GINFO           (1<<0)  /* Do not send GadgetInfo. */
#define BAMF_NO_INTERIM         (1<<1)  /* Skip interim messages.  */

#define BASE_REMMAP                     (BGUI_MB+44)
#define BASE_REMCONDITIONAL             (BGUI_MB+45)
#define BASE_REMMETHOD                  (BGUI_MB+46)

/* Remove an object from a notification list. */
struct bmRemove {
        ULONG                   MethodID;
        Object                 *bar_Object;
};

#define BASE_ADDHOOK                    (BGUI_MB+52)

/* Add a hook to the hook-notification list. */
struct bmAddHook {
        ULONG                   MethodID;
        struct Hook            *bah_Hook;
};

/* Remove a hook from the hook-notification list. */
#define BASE_REMHOOK                    (BGUI_MB+53)


/*
 *      These are required for backwards compatibility with old code.
 *      Use the new identifiers instead.
 */
#define FRQ_Left                ASLREQ_Left
#define FRQ_Top                 ASLREQ_Top
#define FRQ_Width               ASLREQ_Width
#define FRQ_Height              ASLREQ_Height
#define FRQ_Drawer              FILEREQ_Drawer
#define FRQ_File                FILEREQ_File
#define FRQ_Pattern             FILEREQ_Pattern
#define FRQ_Path                FILEREQ_Path
#define FRQ_MultiHook           FILEREQ_MultiHook
#define FRQ_OK                  ASLREQ_OK
#define FRQ_CANCEL              ASLREQ_CANCEL
#define FRQ_ERROR_NO_MEM        ASLREQ_ERROR_NO_MEM
#define FRQ_ERROR_NO_FREQ       ASLREQ_ERROR_NO_REQ
#define FRM_DOREQUEST           ASLM_DOREQUEST


