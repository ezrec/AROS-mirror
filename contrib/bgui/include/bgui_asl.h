#ifndef BGUI_ASL_H
#define BGUI_ASL_H
/*
 * @(#) $Header$
 *
 * $VER: bgui/bgui_asl.h 41.10 (25.4.96)
 * Asl requester class structures and constants.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:23:07  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:45  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:13:43  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:15:45  mlemos
 * Ian sources
 *
 *
 */

#ifndef LIBRARIES_ASL_H
#include <libraries/asl.h>
#endif /* LIBRARIES_ASL_H */

/*****************************************************************************
 *
 *      "aslreqclass" - BOOPSI Asl filerequester classes (file, font, screen)
 *
 *      Tags: 1941 - 2020               Methods: 701 - 740
 */
#define ASLREQ_TAGSTART                 (BGUI_TB+1941)
#define FILEREQ_Drawer                  (BGUI_TB+1941)  /* ISG-- */
#define FILEREQ_File                    (BGUI_TB+1942)  /* ISG-- */
#define FILEREQ_Pattern                 (BGUI_TB+1943)  /* ISG-- */
#define FILEREQ_Path                    (BGUI_TB+1944)  /* --G-- */
#define ASLREQ_Left                     (BGUI_TB+1945)  /* --G-- */
#define ASLREQ_Top                      (BGUI_TB+1946)  /* --G-- */
#define ASLREQ_Width                    (BGUI_TB+1947)  /* --G-- */
#define ASLREQ_Height                   (BGUI_TB+1948)  /* --G-- */
#define FILEREQ_MultiHook               (BGUI_TB+1949)  /* IS--- */  /* V40 */
#define ASLREQ_Type                     (BGUI_TB+1950)  /* I-G-- */  /* V41 */
#define ASLREQ_Requester                (BGUI_TB+1951)  /* --G-- */  /* V41 */
#define ASLREQ_Bounds                   (BGUI_TB+1952)  /* IS--- */  /* V41.8 */

#define FONTREQ_TextAttr                (BGUI_TB+1980)  /* ISG-- */  /* V41 */
#define FONTREQ_Name                    (BGUI_TB+1981)  /* ISG-- */  /* V41 */
#define FONTREQ_Size                    (BGUI_TB+1982)  /* ISG-- */  /* V41 */
#define FONTREQ_Style                   (BGUI_TB+1983)  /* ISG-- */  /* V41 */
#define FONTREQ_Flags                   (BGUI_TB+1984)  /* ISG-- */  /* V41 */
#define FONTREQ_FrontPen                (BGUI_TB+1985)  /* ISG-- */  /* V41 */
#define FONTREQ_BackPen                 (BGUI_TB+1986)  /* ISG-- */  /* V41 */
#define FONTREQ_DrawMode                (BGUI_TB+1987)  /* ISG-- */  /* V41 */

#define SCREENREQ_DisplayID             (BGUI_TB+1990)  /* ISG-- */  /* V41 */
#define SCREENREQ_DisplayWidth          (BGUI_TB+1991)  /* ISG-- */  /* V41 */
#define SCREENREQ_DisplayHeight         (BGUI_TB+1992)  /* ISG-- */  /* V41 */
#define SCREENREQ_DisplayDepth          (BGUI_TB+1993)  /* ISG-- */  /* V41 */
#define SCREENREQ_OverscanType          (BGUI_TB+1994)  /* ISG-- */  /* V41 */
#define SCREENREQ_AutoScroll            (BGUI_TB+1995)  /* ISG-- */  /* V41 */

/*
 *      In addition to the above defined attributes are all
 *      ASL filerequester attributes ISG-U.
 */

/*
 *      Error codes which the SetAttrs() and DoMethod()
 *      calls can return.
 */
#define ASLREQ_OK               (0L)    /* OK. No problems.                 */
#define ASLREQ_CANCEL           (1L)    /* The requester was cancelled.     */
#define ASLREQ_ERROR_NO_MEM     (2L)    /* Out of memory.                   */
#define ASLREQ_ERROR_NO_REQ     (3L)    /* Unable to allocate a requester.  */

/* New Methods */

#define ASLM_DOREQUEST                  (BGUI_MB+701)   /* Show Requester.  */
/*
 * The following three methods are only needed by class implementors.
 */
#define ASLM_ALLOCREQUEST               (BGUI_MB+702)   /* AllocRequester() */
#define ASLM_REQUEST                    (BGUI_MB+703)   /* Request()        */
#define ASLM_FREEREQUEST                (BGUI_MB+704)   /* FreeRequester()  */

#endif /* BGUI_ASL_H */
