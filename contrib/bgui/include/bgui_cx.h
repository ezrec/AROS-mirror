#ifndef BGUI_CX_H
#define BGUI_CX_H
/*
 * @(#) $Header$
 *
 * $VER: bgui/bgui_cx.h 41.10 (25.4.96)
 * Commodity class structures and constants.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:23:09  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:47  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:13:45  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:15:47  mlemos
 * Ian sources
 *
 *
 */

#ifndef LIBRARIES_COMMODITIES_H
#include <libraries/commodities.h>
#endif /* LIBRARIES_COMMODITIES_H */

/*****************************************************************************
 *
 *      "commodityclass" - BOOPSI commodity class.
 *
 *      Tags: 1861 - 1940               Methods: 661 - 700
 */
#define COMM_Name                       (BGUI_TB+1861)  /* I---- */
#define COMM_Title                      (BGUI_TB+1862)  /* I---- */
#define COMM_Description                (BGUI_TB+1863)  /* I---- */
#define COMM_Unique                     (BGUI_TB+1864)  /* I---- */
#define COMM_Notify                     (BGUI_TB+1865)  /* I---- */
#define COMM_ShowHide                   (BGUI_TB+1866)  /* I---- */
#define COMM_Priority                   (BGUI_TB+1867)  /* I---- */
#define COMM_SigMask                    (BGUI_TB+1868)  /* --G-- */
#define COMM_ErrorCode                  (BGUI_TB+1869)  /* --G-- */

/* New Methods. */

#define CM_ADDHOTKEY                    (BGUI_MB+661)

/* Add a hot-key to the broker. */
struct cmAddHotkey {
        ULONG           MethodID;               /* CM_ADDHOTKEY             */
        STRPTR          cah_InputDescription;   /* Key input description.   */
        ULONG           cah_KeyID;              /* Key command ID.          */
        ULONG           cah_Flags;              /* See below.               */
};

/* Flags. */
#define CAHF_DISABLED   (1<<0)  /* The key is added but won't work.         */

#define CM_REMHOTKEY                    (BGUI_MB+662) /* Remove a key.      */
#define CM_DISABLEHOTKEY                (BGUI_MB+663) /* Disable a key.     */
#define CM_ENABLEHOTKEY                 (BGUI_MB+664) /* Enable a key.      */

/* Do a key command. */
struct cmDoKeyCommand {
        ULONG           MethodID;       /* See above.                       */
        ULONG           cdkc_KeyID;     /* ID of the key.                   */
};

#define CM_ENABLEBROKER                 (BGUI_MB+665) /* Enable broker.     */
#define CM_DISABLEBROKER                (BGUI_MB+666) /* Disable broker.    */

#define CM_MSGINFO                      (BGUI_MB+667)

/* Obtain info from a CxMsg. */
struct cmMsgInfo {
        ULONG           MethodID;       /* CM_MSGINFO                       */
        struct {
                ULONG  *Type;           /* Storage for CxMsgType() result.  */
                ULONG  *ID;             /* Storage for CxMsgID() result.    */
                ULONG  *Data;           /* Storage for CxMsgData() result.  */
        }               cmi_Info;
};

/* Possible CM_MSGINFO return codes. */
#define CMMI_NOMORE             (~0L)   /* No more messages.                */
#define CMMI_KILL               (1<<16) /* Remove yourself.         V40     */
#define CMMI_DISABLE            (2<<16) /* You have been disabled.  V40     */
#define CMMI_ENABLE             (3<<16) /* You have been enabled.   V40     */
#define CMMI_UNIQUE             (4<<16) /* Unique violation ocured. V40     */
#define CMMI_APPEAR             (5<<16) /* Show yourself.           V40     */
#define CMMI_DISAPPEAR          (6<<16) /* Hide yourself.           V40     */

/*
 *      CM_ADDHOTKEY error codes obtainable using
 *      the COMM_ErrorCode attribute.
 */
#define CMERR_OK                (0L)    /* OK. No problems.                 */
#define CMERR_NO_MEMORY         (1L)    /* Out of memory.                   */
#define CMERR_KEYID_IN_USE      (2L)    /* Key ID already used.             */
#define CMERR_KEY_CREATION      (3L)    /* Key creation failure.            */
#define CMERR_CXOBJERROR        (4L)    /* CxObjError() reported failure.   */

#endif /* BGUI_CX_H */

