#ifndef _MSG_H
#define _MSG_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

/****************************************************************************/

#define CATNAME "Urltext.catalog"

/****************************************************************************/

/*
 * Codes definition
 */

enum
{
    Msg_Colours = 0,
    Msg_Out,
    Msg_Out_Help,
    Msg_OutPenTitle,
    Msg_Over,
    Msg_Over_Help,
    Msg_OverPenTitle,
    Msg_Visited,
    Msg_Visited_Help,
    Msg_VisitedPenTitle,
    Msg_Settings,
    Msg_Underline,
    Msg_Underline_Help,
    Msg_Fallback,
    Msg_Fallback_Help,
    Msg_DoVisitedPen,
    Msg_DoVisitedPen_Help,
    Msg_Examples,
    Msg_Copyright,
    Msg_Font,
    Msg_Font_Help,
};

/****************************************************************************/

#endif /* _MSG_H */
