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




#define GRM_ADDMEMBER                   (BGUI_MB+81)

/* Add a member to the group. */
struct grmAddMember {
        ULONG                   MethodID;       /* GRM_ADDMEMBER            */
        Object                 *grma_Member;    /* Object to add.           */
        ULONG                   grma_Attr;      /* First of LGO attributes. */
};

#define GRM_REMMEMBER                   (BGUI_MB+82)

/* Remove a member from the group. */
struct grmRemMember {
        ULONG                   MethodID;       /* GRM_REMMEMBER            */
        Object                 *grmr_Member;    /* Object to remove.        */
};

#define GRM_DIMENSIONS                  (BGUI_MB+83)

/* Ask an object it's dimensions information. */
struct grmDimensions {
        ULONG                   MethodID;       /* GRM_DIMENSIONS           */
        struct GadgetInfo      *grmd_GInfo;     /* Can be NULL!             */
        struct RastPort        *grmd_RPort;     /* Ready for calculations.  */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_MinSize;   /* Storage for dimensions.  */
        ULONG                   grmd_Flags;     /* See below.               */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_MaxSize;   /* Storage for dimensions.  */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_NomSize;   /* Storage for dimensions.  */
};

/* Flags */
#define GDIMF_MAXIMUM           (1<<4)  /* The grmd_MaxSize is requested.       */
#define GDIMF_NOMINAL           (1<<3)  /* The grmd_NomSize is requested.       */


#define GRM_ADDSPACEMEMBER              (BGUI_MB+84)

/* Add a weight controlled spacing member. */
struct grmAddSpaceMember {
        ULONG                   MethodID;       /* GRM_ADDSPACEMEMBER       */
        ULONG                   grms_Weight;    /* Object weight.           */
};

#define GRM_INSERTMEMBER                (BGUI_MB+85)

/* Insert a member in the group. */
struct grmInsertMember {
        ULONG                   MethodID;       /* GRM_INSERTMEMBER         */
        Object                 *grmi_Member;    /* Member to insert.        */
        Object                 *grmi_Pred;      /* Insert after this member */
        ULONG                   grmi_Attr;      /* First of LGO attributes. */
};

#define GRM_REPLACEMEMBER               (BGUI_MB+86)    /* V40 */

/* Replace a member in the group. */
struct grmReplaceMember {
        ULONG                   MethodID;       /* GRM_REPLACEMEMBER        */
        Object                 *grrm_MemberA;   /* Object to replace.       */
        Object                 *grrm_MemberB;   /* Object which replaces.   */
        ULONG                   grrm_Attr;      /* First of LGO attributes. */
};





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


