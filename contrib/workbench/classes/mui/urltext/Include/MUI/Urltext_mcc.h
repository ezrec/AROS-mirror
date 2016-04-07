#ifndef URLTEXT_MCC_H
#define URLTEXT_MCC_H

/*
**  $VER: Urltext.h 15.4 (25.1.2002)
**  Includes Release 15.4
**
**  Urltext.mcc
**  Active Url MUI class
**
**  (C) 2000-2002 Alfonso Ranieri <alforan@tin.it>
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
#define UrltextObject      MUIOBJMACRO_START(MUIC_Urltext)

/***********************************************************************/

/* Attributes - Scheme is: [ISGN] */
#define MUIA_Urltext_Url            0xFEC900D0ul /* [I.GN] (STRPTR) */
#define MUIA_Urltext_Text           0xFEC900D1ul /* [I.G.] (STRPTR) */
#define MUIA_Urltext_Active         0xFEC900D2ul /* [..G.] (BOOL)   */
#define MUIA_Urltext_Visited        0xFEC900D3ul /* [..GN] (BOOL)   */
#define MUIA_Urltext_Underline      0xFEC900D4ul /* [I...] (BOOL)   */
#define MUIA_Urltext_FallBack       0xFEC900D5ul /* [I...] (BOOL)   */
#define MUIA_Urltext_DoVisitedPen   0xFEC900D6ul /* [I...] (BOOL)   */
#define MUIA_Urltext_SetMax         0xFEC900D7ul /* [I...] (BOOL)   */
#define MUIA_Urltext_DoOpenURL      0xFEC900D8ul /* [I...] (BOOL)   */
#define MUIA_Urltext_NoMenu         0xFEC900D9ul /* [I...] (BOOL)   */
#define MUIA_Urltext_NoOpenURLPrefs 0xFEC900DCul /* [I...] (BOOL)   */

/***********************************************************************/

/* Methods */
#define MUIM_Urltext_OpenURL        0xFEC900C9ul /* struct MUIP_Urltext_OpenURL */
#define MUIM_Urltext_Copy           0xFEC900CAul /* struct MUIP_Urltext_Copy    */

struct MUIP_Urltext_OpenURL
{
    STACKED ULONG   MethodID;
    STACKED ULONG   flags;      /* Private, PUT 0 */
};

struct MUIP_Urltext_Copy
{
    STACKED ULONG   MethodID;
    STACKED ULONG   unit;
};

/***********************************************************************/

#endif /* URLTEXT_MCC_H */
