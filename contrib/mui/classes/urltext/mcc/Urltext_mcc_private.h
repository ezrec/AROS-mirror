#ifndef _URLTEXT_PRIVATE_MCC_H
#define _URLTEXT_PRIVATE_MCC_H

/*
**  $VER: Urltext_private.h 15.0 (16.12.2001)
**  Includes Release 15.0
**
**  Urltext.mcc
**  Active Url MUI class
**
**  (C) 2000-2001 Alfonso Ranieri <alforan@tin.it>
**  All Rights Reserved
**
*/

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifndef __AROS__
#define STACKED
#endif

/***********************************************************************/

#define MUIC_Urltext       "Urltext.mcc"
#define UrltextObject      MUI_NewObject(MUIC_Urltext

/***********************************************************************/

#define MUISN_Alfie     ((unsigned int) 0xFEC9)
#define TAG_MUI_Alfie   (TAG_USER|(MUISN_Alfie<<16))

/***********************************************************************/

/* attributes - Scheme is: [ISGN] */
enum
{
    MUIA_Urltext_Base = (unsigned int)(TAG_MUI_Alfie+200),

    MUIA_Urltext_MouseOutPen,       /* [IS..] (struct MUI_PenSpec *) PRIVATE!           */
    MUIA_Urltext_MouseOverPen,      /* [IS..] (struct MUI_PenSpec *) PRIVATE!           */
    MUIA_Urltext_VisitedPen,        /* [IS..] (struct MUI_PenSpec *) PRIVATE!           */
    MUIA_Urltext_MouseOver,         /* [.S.N] (BOOL)                 PRIVATE!           */
    MUIA_Urltext_PUnderline,        /* [.S..] (BOOL)                 PRIVATE!           */
    MUIA_Urltext_PDoVisitedPen,     /* [.S..] (BOOL)                 PRIVATE!           */
    MUIA_Urltext_PFallBack,         /* [.S..] (BOOL)                 PRIVATE!           */

    MUIA_Urltext_Url,               /* [I.GN] (STRPTR)                                  */
    MUIA_Urltext_Text,              /* [I.G.] (STRPTR)                                  */
    MUIA_Urltext_Active,            /* [..G.] (BOOL)                                    */
    MUIA_Urltext_Visited,           /* [..GN] (BOOL)                                    */
    MUIA_Urltext_Underline,         /* [I...] (BOOL)                                    */
    MUIA_Urltext_FallBack,          /* [I...] (BOOL)                                    */
    MUIA_Urltext_DoVisitedPen,      /* [I...] (BOOL)                                    */
    MUIA_Urltext_SetMax,            /* [I...] (BOOL)                                    */
    MUIA_Urltext_DoOpenURL,         /* [I...] (BOOL)                                    */
    MUIA_Urltext_NoMenu,            /* [I...] (BOOL)                                    */

    MUIA_Urltext_Font,              /* PRIVATE!                                         */
    MUIA_Urltext_Version,           /* PRIVATE!                                         */
};

/***********************************************************************/

/* methods */
enum
{
    MUIAM_Urltext_Base = (unsigned int)(TAG_MUI_Alfie+200),
    MUIM_Urltext_OpenURL,
    MUIM_Urltext_Copy,
    MUIM_Urltext_OpenURLPrefs,
};

/***********************************************************************/

struct MUIP_Urltext_OpenURL
{
    STACKED ULONG   MethodID;
    STACKED ULONG   flags;
};

#define MUIV_Urltext_OpenURL_CheckOver  0x00000001

struct MUIP_Urltext_Copy
{
    STACKED ULONG   MethodID;
    STACKED ULONG   unit;
};

/***********************************************************************/

/* Urltext defaults */
#define DEFAULT_MOUSEOUT_PEN    MPEN_FILL
#define DEFAULT_MOUSEOVER_PEN   MPEN_SHINE
#define DEFAULT_VISITED_PEN     MPEN_MARK
#define DEFAULT_UNDERLINE       1
#define DEFAULT_DOVISITEDPEN    1
#define DEFAULT_FALLBACK        1
#define DEFAULT_SETMAX          1

/***********************************************************************/

struct data
{
    char                        url[256];
    char                        text[256];
    int                         tLen;
    struct MUI_RenderInfo       *mri;
    struct TextExtent           te;
    Object                      *menu;
    struct TextFont             *tf;
    ULONG                       flags;
    LONG                        mouseOutPen;
    LONG                        mouseOverPen;
    LONG                        visitedPen;
    LONG                        textpen;
    struct MUI_EventHandlerNode he;
};

#endif /* _URLTEXT_PRIVATE_MCC_H */
