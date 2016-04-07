
#include "class.h"

/****************************************************************************/
/*
 * Strings definitions
 */

#define Msg_Colours_STR             "Colours"
#define Msg_Out_STR                 "_Out"
#define Msg_Out_Help_STR            "Pen when the mouse is not over the link."
#define Msg_OutPenTitle_STR         "Choose 'Out' pen"
#define Msg_Over_STR                "O_ver"
#define Msg_Over_Help_STR           "Pen when the mouse is over the link."
#define Msg_OverPenTitle_STR        "Choose 'Over' pen"
#define Msg_Visited_STR             "Visi_ted"
#define Msg_Visited_Help_STR        "Pen for a visited link."
#define Msg_VisitedPenTitle_STR     "Choose 'Visited' pen"
#define Msg_Settings_STR            "Settings"
#define Msg_Underline_STR           "Underl_ine"
#define Msg_Underline_Help_STR      "Underline link."
#define Msg_Fallback_STR            "_Fallback"
#define Msg_Fallback_Help_STR       "Active even if OpenURL\nis not installed."
#define Msg_DoVisitedPen_STR        "Us_e Visited Pen"
#define Msg_DoVisitedPen_Help_STR   "Use 'Visited' pen for a visited link."
#define Msg_Examples_STR            "Examples"
#define Msg_Copyright_STR           "Copyright 2000-2002 Alfonso Ranieri <alforan@tin.it>"
#define Msg_Font_STR                "Fo_nt"
#define Msg_Font_Help_STR           "The font to use."

/****************************************************************************/
/*
 * Code/String array definition
 */

struct message
{
    LONG   id;
    STRPTR string;
};

struct message messages[] =
{
    {Msg_Colours,            Msg_Colours_STR},
    {Msg_Out,                Msg_Out_STR},
    {Msg_Out_Help,           Msg_Out_Help_STR},
    {Msg_OutPenTitle,        Msg_OutPenTitle_STR},
    {Msg_Over,               Msg_Over_STR},
    {Msg_Over_Help,          Msg_Over_Help_STR},
    {Msg_OverPenTitle,       Msg_OverPenTitle_STR},
    {Msg_Visited,            Msg_Visited_STR},
    {Msg_Visited_Help,       Msg_Visited_Help_STR},
    {Msg_VisitedPenTitle,    Msg_VisitedPenTitle_STR},
    {Msg_Settings,           Msg_Settings_STR},
    {Msg_Underline,          Msg_Underline_STR},
    {Msg_Underline_Help,     Msg_Underline_Help_STR},
    {Msg_Fallback,           Msg_Fallback_STR},
    {Msg_Fallback_Help,      Msg_Fallback_Help_STR},
    {Msg_DoVisitedPen,       Msg_DoVisitedPen_STR},
    {Msg_DoVisitedPen_Help,  Msg_DoVisitedPen_Help_STR},
    {Msg_Examples,           Msg_Examples_STR},
    {Msg_Copyright,          Msg_Copyright_STR},
    {Msg_Font,               Msg_Font_STR},
    {Msg_Font_Help,          Msg_Font_Help_STR},

    { 0 }
};

/****************************************************************************/

CONST_STRPTR ASM
getString(REG(d0) ULONG id)
{
    struct message *array;
    CONST_STRPTR    string;

    for (array = messages;
         array->string && (array->id!=id);
         array++);

    string = array->string;
    if (UrltextBase->cat) string = GetCatalogStr(UrltextBase->cat,id,string);

    return string;
}

/****************************************************************************/
