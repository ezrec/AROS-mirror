/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

 Sourcecode for Mailtext.mcp

***************************************************************************/

/* --- Includes --- */

#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/locale.h>
#include <utility/tagitem.h>
#include <string.h>
#ifdef __AROS__
#include <MUI/NListview_mcc.h>
#else
#include <mui/NListview_mcc.h>
#endif

#include "Mailtext_private.h"
#include "loc/Mailtext_mcp.h"
#ifdef __AROS__
#include "aros/MailtextVersionsAROS.h"
#else
#include "rev/MailtextVersions.h"
#endif

#define CLASS         MUIC_MailtextP
#define MUIMINVERSION 17

#ifdef __AROS__
#include "aros/Mailtext_mcp_private.h"
#else
struct Data
{
            char    *regTitles[7] ;
            char    *text ;

            Object  *mailtextObj,
                    *distObj,
                    *qdistObj,
                    *disPatObj,
                    *enPatObj,
                    *ssPatObj,
                    *niCharsObj,
                    *termCharsObj,
                    *wsCharsObj,
                    *qCharsObj,
                    *showObj,
                    *contextObj,
                    *uQCharsObj,
                    *colors[5],
                    *textcolor,
                    *fontObj,
                    *urlcolor,
                    *group,
                    *sigSepObj,
                    *uulObj,
                    *ustackObj,
                    *uactStrObj,
                    *uabeepObj,
                    *wrapObj ;

    struct  Catalog *catalog;
};
#endif

struct Library *LocaleBase;
#if defined(__amigaos4__)
struct LocaleIFace  * ILocale     = NULL;
//struct UtilityIFace * IUtility    = NULL;
//struct UtilityBase  * UtilityBase = NULL;
#else
//struct Library *UtilityBase;
#endif

#ifdef __AROS__
#else
#include "rev/Mailtext.mcp.h"
#include "rev/Mailtext.dist.h"
#endif

#ifndef MUII_PopFont
#define MUII_PopFont 42
#endif
#ifndef MUIM_Mccprefs_RegisterGadget
#define MUIM_Mccprefs_RegisterGadget 0x80424828 /* V20 */
#endif

#define USE_PREFSIMAGE_COLORS
#define USE_PREFSIMAGE_BODY
#include "mui/PrefsImage.c"

#ifndef __AROS__
#include <mui/mccbase.c>
#endif

/* --- Support-Functions */

/*/// "LONG GetControlChar(char *string)" */

static LONG GetControlChar(char *string)
{
    char *ptr;

    if (ptr=strchr(string,'_'))
    {
        if (strlen(ptr)>1)
        {
            return (LONG)ToLower((ULONG)*(++ptr)) ;
        }
    }

    return (LONG)TAG_IGNORE ;
}

/*\\\*/

/* --- Methods --- */

/*/// "static ULONG New(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR MailtextP__OM_NEW(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG New(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    struct Data *data;

    Object *crawl, *lv, *temp1, *temp2, *temp3, *temp4, *temp5, *temp6 ;

    if(!(obj = (Object *)DoSuperMethodA(cl, obj, msg)))
    {
        return(0);
    }

    /*** init data ***/
    data = INST_DATA(cl,obj);

    data->catalog = OpenMailtext_mcpCatalog(NULL, "") ;

    data->regTitles[0] = GetString(data->catalog, MSG_Distances) ;
    data->regTitles[1] = GetString(data->catalog, MSG_Patterns) ;
    data->regTitles[2] = GetString(data->catalog, MSG_Chars) ;
    data->regTitles[3] = GetString(data->catalog, MSG_Colors) ;
    data->regTitles[4] = GetString(data->catalog, MSG_URLs) ;
    data->regTitles[5] = GetString(data->catalog, MSG_Misc) ;
    data->regTitles[6] = NULL;

    /* make object for (C) notice */

    if (!(crawl = MUI_NewObject("Crawling.mcc", MUIA_Frame, MUIV_Frame_Text,
                                                MUIA_FixHeightTxt, "\n",
                                                MUIA_Background, MUII_TextBack,
                                                MUIA_Virtgroup_Input, FALSE,

                                                Child, VGroup,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, __DCOPYRIGHT "\n\nMailtext.mcc v" __MCCVERSION "\nMailtext.mcp v" __MCPVERSION "\n",
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, GetString(data->catalog, MSG_InfoGiftware),
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, "http://sourceforge.net/projects/mailtext/\n",

                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, GetString(data->catalog, MSG_InfoDeveloper),
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, "The Mailtext OpenSource development team\n",
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, GetString(data->catalog, MSG_InfoTranslators),
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,

                                                           Child, TextObject,
                                                                      MUIA_Text_Contents, __DCOPYRIGHT,
                                                                      MUIA_Text_PreParse, MUIX_C,
                                                                  End,
                                                       End,
                       TAG_DONE)))
    {
        crawl = TextObject,
                    MUIA_Text_Contents, __DCOPYRIGHT,
                    MUIA_Text_PreParse,  MUIX_C,
                End;

        SetAttrs(crawl, MUIA_Frame, MUIV_Frame_Text,
                        MUIA_Background, MUII_TextBack,
                 TAG_DONE) ;

    }



    /*** make group ***/

    data->group = VGroup,

        /* Sample group */

        Child, VGroup,

            Child, MUI_MakeObject(MUIO_BarTitle, GetString(data->catalog, MSG_Sample)),

            Child, lv = NListviewObject,
                            MUIA_NListview_NList, data->mailtextObj = MailtextObject,
                                                                          MUIA_Frame, MUIV_Frame_InputList,
                                                                          MUIA_Font,  MUIV_NList_Font_Fixed,
                                                                          MUIA_Mailtext_Text, data->text = GetString(data->catalog, MSG_SampleText),
                                                                          MUIA_NList_Input, FALSE,
                                                                      End,
                            MUIA_CycleChain,     TRUE,
                            MUIA_ShortHelp,      GetString(data->catalog, MSG_SampleHelp),
                        End,

            Child, MUI_MakeObject(MUIO_BarTitle, GetString(data->catalog, MSG_Preferences)),

        End,

        /* Register group */

        Child, RegisterObject,
            MUIA_Register_Titles, data->regTitles,
            MUIA_Register_Frame,  TRUE,
            MUIA_Weight,          0,
            MUIA_CycleChain,      TRUE,

            /* Distances */

            Child, ColGroup(2),

                Child, Label2(GetString(data->catalog, MSG_AttributesDistance)),
                Child, data->distObj = SliderObject,
                                           MUIA_Numeric_Default, MUID_Mailtext_Distance,
                                           MUIA_Numeric_Max,     200,
                                           MUIA_Numeric_Min,     0,
                                           MUIA_CycleChain,      TRUE,
                                           MUIA_ShortHelp,       GetString(data->catalog, MSG_AttributesDistanceHelp),
                                           MUIA_ControlChar,     GetControlChar(GetString(data->catalog, MSG_AttributesDistance)),
                                       End,

                Child, Label2(GetString(data->catalog, MSG_QuoteDistance)),
                Child, data->qdistObj = SliderObject,
                                            MUIA_Numeric_Default, MUID_Mailtext_QuoteDistance,
                                            MUIA_Numeric_Max,     40,
                                            MUIA_Numeric_Min,     0,
                                            MUIA_CycleChain,      TRUE,
                                            MUIA_ShortHelp,       GetString(data->catalog, MSG_QuoteDistanceHelp),
                                            MUIA_ControlChar,     GetControlChar(GetString(data->catalog, MSG_QuoteDistance)),
                                        End,
            End,

            /* Patterns  */

            Child, ColGroup(2),

                Child, Label2(GetString(data->catalog, MSG_DisablePattern)),
                Child, data->disPatObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_DisablePattern), 512),


                Child, Label2(GetString(data->catalog, MSG_EnablePattern)),
                Child, data->enPatObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_EnablePattern), 512),

                Child, Label2(GetString(data->catalog, MSG_SigSepPattern)),
                Child, data->ssPatObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_SigSepPattern), 512),

            End,

            /* Chars */

            Child, ColGroup(5),

                Child, Label2(GetString(data->catalog, MSG_NonintroducingChars)),
                Child, data->niCharsObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_NonintroducingChars), 50),

                Child, Label2(GetString(data->catalog, MSG_TerminatingChars)),
                Child, data->termCharsObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_TerminatingChars), 50),
                Child, HSpace(0),

                Child, Label2(GetString(data->catalog, MSG_WhitespaceChars)),
                Child, data->wsCharsObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_WhitespaceChars), 50),

                Child, Label2(GetString(data->catalog, MSG_QuoteChars)),
                Child, data->qCharsObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_QuoteChars), 10),
                Child, data->uQCharsObj = MUI_MakeObject(MUIO_Checkmark, NULL),

            End,

            /* Colors */

            Child, VGroup,
                Child, VSpace(0),

                Child, ColGroup(2),
                           Child, FreeLabel(GetString(data->catalog, MSG_Quotelevel)),

                           Child, HGroup,

                                      Child, FreeLabel("_1 ="),
                                      Child, data->colors[0] = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_QuoteLevel1Color),
                                                                   MUIA_ControlChar,  '1',
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_QuoteLevelHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                      Child, FreeLabel("_2 ="),
                                      Child, data->colors[1] = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_QuoteLevel2Color),
                                                                   MUIA_ControlChar,  '2',
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_QuoteLevelHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                      Child, FreeLabel("_3 ="),
                                      Child, data->colors[2] = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_QuoteLevel3Color),
                                                                   MUIA_ControlChar,  '3',
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_QuoteLevelHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                      Child, FreeLabel("_4 ="),
                                      Child, data->colors[3] = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_QuoteLevel4Color),
                                                                   MUIA_ControlChar,  '4',
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_QuoteLevelHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                      Child, FreeLabel("_5 ="),
                                      Child, data->colors[4] = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_QuoteLevel5Color),
                                                                   MUIA_ControlChar,  '5',
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_QuoteLevelHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                  End,

                           Child, VSpace(0),
                           Child, VSpace(0),

                           Child, FreeLabel(GetString(data->catalog, MSG_ColoredText)),

                           Child, HGroup,
                                      Child, data->textcolor = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_ColoredTextWinTitle),
                                                                   MUIA_ControlChar,  GetControlChar(GetString(data->catalog, MSG_ColoredText)),
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_Weight,       100,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_ColoredTextHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                      Child, HSpace(0),

                                      Child, FreeLabel(GetString(data->catalog, MSG_ColoredURL)),
                                      Child, data->urlcolor = PoppenObject,
                                                                   MUIA_Window_Title, GetString(data->catalog, MSG_ColoredURLWinTitle),
                                                                   MUIA_ControlChar,  GetControlChar(GetString(data->catalog, MSG_ColoredURL)),
                                                                   MUIA_Draggable,    TRUE,
                                                                   MUIA_Weight,       100,
                                                                   MUIA_ShortHelp,    GetString(data->catalog, MSG_ColoredURLHelp),
                                                                   MUIA_CycleChain,   TRUE,
                                                               End,

                                  End,
                       End,

                Child, VSpace(0),

            End,

            /* URLs */

            Child, VGroup,
                       Child, VSpace(0),

                       Child, ColGroup(2),
                                  Child, Label2(GetString(data->catalog, MSG_URLAction)),
                                  Child, PopaslObject,
                                             MUIA_Popstring_String, data->uactStrObj = MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_URLAction), 128),
                                             MUIA_Popstring_Button, PopButton(MUII_PopFile),
                                         End,

                                  Child, Label2(GetString(data->catalog, MSG_URLStackSize)),
                                  Child, HGroup,
                                             Child, data->ustackObj = MUI_MakeObject(MUIO_NumericButton, GetString(data->catalog, MSG_URLStackSize), 4, 128, GetString(data->catalog, MSG_URLStackSlider)),
                                             Child, HSpace(0),
                                             Child, MUI_MakeObject(MUIO_VBar, 2),
                                             Child, HSpace(0),
                                             Child, data->uabeepObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_URLActionBeep)),
                                             Child, temp1 = LLabel1(GetString(data->catalog, MSG_URLActionBeep)),
                                             Child, HSpace(0),
                                         End,
                              End,

                       Child, VSpace(0),
            End,

            /* Misc */

            Child, VGroup,
                       Child, VSpace(0),

                       Child, ColGroup(7),
                                  Child, HSpace(0),

                                  Child, ColGroup(2),
                                             Child, data->showObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_ShowAttributes)),
                                             Child, temp2 = LLabel1(GetString(data->catalog, MSG_ShowAttributes)),

                                             Child, data->uulObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_URLUnderline)),
                                             Child, temp3 = LLabel1(GetString(data->catalog, MSG_URLUnderline)),

                                             Child, data->wrapObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_Wordwrap)),
                                             Child, temp6 = LLabel1(GetString(data->catalog, MSG_Wordwrap)),

                                         End,

                                  Child, HSpace(0),

                                  Child, MUI_MakeObject(MUIO_VBar, 2),

                                  Child, HSpace(0),

                                  Child, VGroup,
                                             Child, ColGroup(2),
                                                        Child, data->contextObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_UseContextMenu)),
                                                        Child, temp4 = LLabel1(GetString(data->catalog, MSG_UseContextMenu)),

                                                        Child, data->sigSepObj = MUI_MakeObject(MUIO_Checkmark, GetString(data->catalog, MSG_SigSeparator)),
                                                        Child, temp5 = LLabel1(GetString(data->catalog, MSG_SigSeparator)),
                                                    End,

                                             Child, ColGroup(2),
                                                        Child, LLabel1(GetString(data->catalog, MSG_Font)),
                                                        Child, data->fontObj = PopaslObject,
                                                                                   MUIA_Popstring_String, MUI_MakeObject(MUIO_String, GetString(data->catalog, MSG_Font), 48),
                                                                                   MUIA_Popstring_Button, PopButton((MUIMasterBase->lib_Version >= 20 ? MUII_PopFont : MUII_PopUp)),
                                                                                   MUIA_Popasl_Type,      ASL_FontRequest,
                                                                               End,
                                                    End,
                                         End,

                                  Child, HSpace(0),
                              End,

                       Child, VSpace(0),
            End,

        End,

        /* Crawl */

        Child, crawl,

    End;

    if (!data->group)
    {
        CoerceMethod(cl, obj, OM_DISPOSE);
        return(0);
    }

    set(temp1, MUIA_ShortHelp, GetString(data->catalog, MSG_URLActionBeepHelp)) ;
    set(temp2, MUIA_ShortHelp, GetString(data->catalog, MSG_ShowAttributesHelp)) ;
    set(temp3, MUIA_ShortHelp, GetString(data->catalog, MSG_URLUnderlineHelp)) ;
    set(temp4, MUIA_ShortHelp, GetString(data->catalog, MSG_UseContextMenuHelp)) ;
    set(temp5, MUIA_ShortHelp, GetString(data->catalog, MSG_SigSeparatorHelp)) ;
    set(temp6, MUIA_ShortHelp, GetString(data->catalog, MSG_WordwrapHelp)) ;

    SetAttrs(data->sigSepObj, MUIA_CycleChain, TRUE,
                              MUIA_ShortHelp,  GetString(data->catalog, MSG_SigSeparatorHelp),
                              MUIA_Selected,   MUID_Mailtext_SeparateSignature,
             TAG_DONE) ;

    SetAttrs(data->fontObj, MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp,  GetString(data->catalog, MSG_FontHelp),
             TAG_DONE) ;

    SetAttrs(data->wrapObj, MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp,  GetString(data->catalog, MSG_WordwrapHelp),
                            MUIA_Selected,   MUID_Mailtext_Wordwrap,
             TAG_DONE) ;

    SetAttrs(data->uactStrObj, MUIA_String_Contents,    "",
                               MUIA_CycleChain,         TRUE,
                               MUIA_String_AdvanceOnCR, TRUE,
                               MUIA_ShortHelp,          GetString(data->catalog, MSG_URLActionHelp),
             TAG_DONE) ;

    SetAttrs(data->ustackObj, MUIA_Numeric_Value,   MUID_Mailtext_URLActionStack,
                              MUIA_CycleChain,      TRUE,
                              MUIA_ShortHelp,       GetString(data->catalog, MSG_URLStackSizeHelp),
                              MUIA_Numeric_Default, MUID_Mailtext_URLActionStack,
             TAG_DONE) ;

    SetAttrs(data->uulObj, MUIA_Selected,   MUID_Mailtext_URLUnderline,
                           MUIA_CycleChain, TRUE,
                           MUIA_ShortHelp,  GetString(data->catalog, MSG_URLUnderlineHelp),
             TAG_DONE) ;

    SetAttrs(data->uabeepObj, MUIA_Selected,   MUID_Mailtext_URLActionBeep,
                              MUIA_CycleChain, TRUE,
                              MUIA_ShortHelp,  GetString(data->catalog, MSG_URLActionBeepHelp),
             TAG_DONE) ;

    SetAttrs(data->disPatObj, MUIA_String_Contents,    MUID_Mailtext_DisablePattern,
                              MUIA_CycleChain,         TRUE,
                              MUIA_String_AdvanceOnCR, TRUE,
                              MUIA_ShortHelp,          GetString(data->catalog, MSG_DisablePatternHelp),
             TAG_DONE) ;

    SetAttrs(data->enPatObj,  MUIA_String_Contents,    MUID_Mailtext_EnablePattern,
                              MUIA_CycleChain,         TRUE,
                              MUIA_String_AdvanceOnCR, TRUE,
                              MUIA_ShortHelp,          GetString(data->catalog, MSG_EnablePatternHelp),
             TAG_DONE) ;

    SetAttrs(data->ssPatObj,  MUIA_String_Contents,    MUID_Mailtext_SigSepPattern,
                              MUIA_CycleChain,         TRUE,
                              MUIA_String_AdvanceOnCR, TRUE,
                              MUIA_ShortHelp,          GetString(data->catalog, MSG_SigSepPatternHelp),
             TAG_DONE) ;

    SetAttrs(data->niCharsObj, MUIA_String_Contents,    MUID_Mailtext_NonIntroducingChars,
                               MUIA_CycleChain,         TRUE,
                               MUIA_String_AdvanceOnCR, TRUE,
                               MUIA_ShortHelp,          GetString(data->catalog, MSG_NonintroducingCharsHelp),
             TAG_DONE) ;

    SetAttrs(data->qCharsObj, MUIA_String_Contents,    MUID_Mailtext_QuoteChars,
                              MUIA_CycleChain,         TRUE,
                              MUIA_String_AdvanceOnCR, TRUE,
                              MUIA_ShortHelp,          GetString(data->catalog, MSG_QuoteCharsHelp),
                              MUIA_Disabled,           !MUID_Mailtext_QuoteCharsUse,
             TAG_DONE) ;

    SetAttrs(data->uQCharsObj, MUIA_CycleChain,         TRUE,
                               MUIA_String_AdvanceOnCR, TRUE,
                               MUIA_ShortHelp,          GetString(data->catalog, MSG_QuoteCharsCheckHelp),
             TAG_DONE) ;

    SetAttrs(data->termCharsObj, MUIA_String_Contents,    MUID_Mailtext_TerminatingChars,
                                 MUIA_CycleChain,         TRUE,
                                 MUIA_String_AdvanceOnCR, TRUE,
                                 MUIA_ShortHelp,          GetString(data->catalog, MSG_TerminatingCharsHelp),
             TAG_DONE) ;

    SetAttrs(data->wsCharsObj, MUIA_String_Contents,    MUID_Mailtext_WhitespaceChars,
                               MUIA_CycleChain,         TRUE,
                               MUIA_String_AdvanceOnCR, TRUE,
                               MUIA_ShortHelp,          GetString(data->catalog, MSG_WhitespaceCharsHelp),
             TAG_DONE) ;

    SetAttrs(data->showObj, MUIA_Selected,   MUID_Mailtext_ShowAttributes,
                            MUIA_CycleChain, TRUE,
                            MUIA_ShortHelp,  GetString(data->catalog, MSG_ShowAttributesHelp),
             TAG_DONE) ;

    SetAttrs(data->contextObj, MUIA_Selected,   MUID_Mailtext_UseContextMenu,
                               MUIA_CycleChain, TRUE,
                               MUIA_ShortHelp,  GetString(data->catalog, MSG_UseContextMenuHelp),
             TAG_DONE) ;

    DoMethod(obj, OM_ADDMEMBER, data->group) ;

    DoMethod(data->qCharsObj,    MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_QuoteChars, MUIV_TriggerValue) ;

    DoMethod(data->qdistObj,     MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_QuoteDistance, MUIV_TriggerValue) ;

    DoMethod(data->distObj,      MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Distance, MUIV_TriggerValue) ;

    DoMethod(data->disPatObj,    MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_DisablePattern, MUIV_TriggerValue) ;

    DoMethod(data->enPatObj,     MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_EnablePattern, MUIV_TriggerValue) ;

    DoMethod(data->ssPatObj,     MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_SigSepPattern, MUIV_TriggerValue) ;

    DoMethod(data->niCharsObj,   MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_NonIntroducingChars, MUIV_TriggerValue) ;

    DoMethod(data->termCharsObj, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_TerminatingChars, MUIV_TriggerValue) ;

    DoMethod(data->wsCharsObj,   MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_WhitespaceChars, MUIV_TriggerValue) ;

    DoMethod(data->showObj,      MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_ShowAttributes, MUIV_TriggerValue) ;

    DoMethod(data->sigSepObj,    MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_SeparateSignature, MUIV_TriggerValue) ;

    DoMethod(data->contextObj,   MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_UseContextMenu, MUIV_TriggerValue) ;

    DoMethod(data->uulObj,       MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_URLUnderline, MUIV_TriggerValue) ;

    DoMethod(data->uabeepObj,    MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_URLActionBeep, MUIV_TriggerValue) ;

    DoMethod(data->wrapObj,      MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Wordwrap, MUIV_TriggerValue) ;

    DoMethod(data->uactStrObj,   MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_URLAction, MUIV_TriggerValue) ;

    DoMethod(data->ustackObj,    MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, obj,
                                           3, MUIM_Set, MUIA_Mailtext_URLActionStack, MUIV_TriggerValue) ;

    DoMethod(data->colors[0],    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_QuoteLevel1, MUIV_TriggerValue) ;

    DoMethod(data->colors[1],    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_QuoteLevel2, MUIV_TriggerValue) ;

    DoMethod(data->colors[2],    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_QuoteLevel3, MUIV_TriggerValue) ;

    DoMethod(data->colors[3],    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_QuoteLevel4, MUIV_TriggerValue) ;

    DoMethod(data->colors[4],    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_QuoteLevel5, MUIV_TriggerValue) ;

    DoMethod(data->urlcolor,     MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_URL, MUIV_TriggerValue) ;

    DoMethod(data->textcolor,    MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime, data->mailtextObj,
                                           3, MUIM_Set, MUIA_Mailtext_Color_Text, MUIV_TriggerValue) ;

    DoMethod(data->uQCharsObj,   MUIM_Notify, MUIA_Selected, MUIV_EveryTime, data->qCharsObj,
                                           3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue) ;

    if (MUIMasterBase->lib_Version >= 20)
    {
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->showObj,      MUICFG_Mailtext_ShowAttributes,      1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->wrapObj,      MUICFG_Mailtext_Wordwrap,            1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->contextObj,   MUICFG_Mailtext_UseContextMenu,      1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->uQCharsObj,   MUICFG_Mailtext_QuoteCharsUse,       1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->uulObj,       MUICFG_Mailtext_URLUnderline,        1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->uabeepObj,    MUICFG_Mailtext_URLActionBeep,       1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->sigSepObj,    MUICFG_Mailtext_SeparateSignature,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->colors[0],    MUICFG_Mailtext_Color_QuoteLevel1,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->colors[1],    MUICFG_Mailtext_Color_QuoteLevel2,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->colors[2],    MUICFG_Mailtext_Color_QuoteLevel3,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->colors[3],    MUICFG_Mailtext_Color_QuoteLevel4,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->colors[4],    MUICFG_Mailtext_Color_QuoteLevel5,   1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->urlcolor,     MUICFG_Mailtext_Color_URL,           1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->textcolor,    MUICFG_Mailtext_Color_Text,          1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->distObj,      MUICFG_Mailtext_Distance,            1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->qdistObj,     MUICFG_Mailtext_QuoteDistance,       1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->ustackObj,    MUICFG_Mailtext_URLActionStack,      1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->disPatObj,    MUICFG_Mailtext_DisablePattern,      1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->enPatObj,     MUICFG_Mailtext_EnablePattern,       1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->ssPatObj,     MUICFG_Mailtext_SigSepPattern,       1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->niCharsObj,   MUICFG_Mailtext_NonIntroducingChars, 1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->termCharsObj, MUICFG_Mailtext_TerminatingChars,    1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->wsCharsObj,   MUICFG_Mailtext_WhitespaceChars,     1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->qCharsObj,    MUICFG_Mailtext_QuoteChars,          1, NULL) ;
        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, data->uactStrObj,   MUICFG_Mailtext_URLAction,           1, NULL) ;
    }

    return((ULONG)obj);
}

/*\\\*/
/*/// "static ULONG Dispose(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR MailtextP__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG Dispose(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);

    CloseMailtext_mcpCatalog(data->catalog) ;

    return(DoSuperMethodA(cl,obj,msg));
}

/*\\\*/
/*/// "static ULONG Get(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR MailtextP__OM_GET(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG Get(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    ULONG *store = ((struct opGet *)msg)->opg_Storage;

    switch( ((struct opGet *)msg)->opg_AttrID )
    {
        case MUIA_Version :             *store = VERSION ;                       return(TRUE) ; break ;
        case MUIA_Revision :            *store = REVISION ;                      return(TRUE) ; break ;
    }

    return(DoSuperMethodA(cl, obj, msg));
}

/*\\\*/

/*/// "static ULONG ConfigToGadgets(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_ConfigToGadgets *msg)" */

#ifdef __AROS__
IPTR MailtextP__MUIM_Settingsgroup_ConfigToGadgets(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_ConfigToGadgets *msg)
#else
static ULONG ConfigToGadgets(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_ConfigToGadgets *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    ULONG item ;

    /*/// "Boolean values" */

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_ShowAttributes))
    {
        set(data->showObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->showObj, MUIA_Selected, MUID_Mailtext_ShowAttributes) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Wordwrap))
    {
        set(data->wrapObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->wrapObj, MUIA_Selected, MUID_Mailtext_Wordwrap) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_UseContextMenu))
    {
        set(data->contextObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->contextObj, MUIA_Selected, MUID_Mailtext_UseContextMenu) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_QuoteCharsUse))
    {
        set(data->uQCharsObj, MUIA_Selected, *(ULONG *)item) ;
        set(data->qCharsObj,  MUIA_Disabled, *(ULONG *)item == FALSE) ;
    }
    else
    {
        set(data->uQCharsObj, MUIA_Selected, MUID_Mailtext_QuoteCharsUse) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_URLUnderline))
    {
        set(data->uulObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->uulObj, MUIA_Selected, MUID_Mailtext_URLUnderline) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_URLActionBeep))
    {
        set(data->uabeepObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->uabeepObj, MUIA_Selected, MUID_Mailtext_URLActionBeep) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_SeparateSignature))
    {
        set(data->sigSepObj, MUIA_Selected, *(ULONG *)item) ;
    }
    else
    {
        set(data->sigSepObj, MUIA_Selected, MUID_Mailtext_SeparateSignature) ;
    }

    /*\\\*/

    /*/// "Pens" */

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_QuoteLevel1))
    {
        set(data->colors[0], MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->colors[0], MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_QuoteLevel2))
    {
        set(data->colors[1], MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->colors[1], MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_QuoteLevel3))
    {
        set(data->colors[2], MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->colors[2], MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_QuoteLevel4))
    {
        set(data->colors[3], MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->colors[3], MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_QuoteLevel5))
    {
        set(data->colors[4], MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->colors[4], MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_URL))
    {
        set(data->urlcolor, MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->urlcolor, MUIM_Pendisplay_SetMUIPen, MAILTEXT_TEXTPEN_DEFAULT) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Color_Text))
    {
        set(data->textcolor, MUIA_Pendisplay_Spec, item) ;
    }
    else
    {
        DoMethod(data->textcolor, MUIM_Pendisplay_SetMUIPen, MAILTEXT_TEXTPEN_DEFAULT) ;
    }

    /*\\\*/

    /*/// "Numeric values" */

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Distance))
    {
        set(data->distObj, MUIA_Numeric_Value, *(ULONG *)item) ;
    }
    else
    {
        set(data->distObj, MUIA_Numeric_Value, MUID_Mailtext_Distance) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_QuoteDistance))
    {
        set(data->qdistObj, MUIA_Numeric_Value, *(ULONG *)item) ;
    }
    else
    {
        set(data->qdistObj, MUIA_Numeric_Value, MUID_Mailtext_QuoteDistance) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_URLActionStack))
    {
        set(data->ustackObj, MUIA_Numeric_Value, *(ULONG *)item) ;
    }
    else
    {
        set(data->ustackObj, MUIA_Numeric_Value, MUID_Mailtext_URLActionStack) ;
    }

    /*\\\*/

    /*/// "Strings" */

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_Font))
    {
        set(data->fontObj, MUIA_String_Contents, item) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_DisablePattern))
    {
        set(data->disPatObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->disPatObj, MUIA_String_Contents, MUID_Mailtext_DisablePattern) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_EnablePattern))
    {
        set(data->enPatObj, MUIA_String_Contents, item);
    }
    else
    {
        set(data->enPatObj, MUIA_String_Contents, MUID_Mailtext_EnablePattern) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_SigSepPattern))
    {
        set(data->ssPatObj, MUIA_String_Contents, item);
    }
    else
    {
        set(data->ssPatObj, MUIA_String_Contents, MUID_Mailtext_SigSepPattern) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_NonIntroducingChars))
    {
        set(data->niCharsObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->niCharsObj, MUIA_String_Contents, MUID_Mailtext_NonIntroducingChars) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_TerminatingChars))
    {
        set(data->termCharsObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->termCharsObj, MUIA_String_Contents, MUID_Mailtext_TerminatingChars) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_WhitespaceChars))
    {
        set(data->wsCharsObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->wsCharsObj, MUIA_String_Contents, MUID_Mailtext_WhitespaceChars) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_QuoteChars))
    {
        set(data->qCharsObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->qCharsObj, MUIA_String_Contents, MUID_Mailtext_QuoteChars) ;
    }

    if (item = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_Mailtext_URLAction))
    {
        set(data->uactStrObj, MUIA_String_Contents, item) ;
    }
    else
    {
        set(data->uactStrObj, MUIA_String_Contents, "") ;
    }

    /*\\\*/

    return(0);
}

/*\\\*/
/*/// "static ULONG GadgetsToConfig(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_GadgetsToConfig *msg)" */

#ifdef __AROS__
IPTR MailtextP__MUIM_Settingsgroup_GadgetsToConfig(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_GadgetsToConfig *msg)
#else
static ULONG GadgetsToConfig(struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_GadgetsToConfig *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    ULONG item ;

    /*/// "Boolean values" */

    get(data->showObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_ShowAttributes) ;

    get(data->wrapObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_Wordwrap) ;

    get(data->contextObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_UseContextMenu) ;

    get(data->uQCharsObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_QuoteCharsUse) ;

    get(data->uulObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_URLUnderline) ;

    get(data->uabeepObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_URLActionBeep) ;

    get(data->sigSepObj, MUIA_Selected, &item) ;
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_SeparateSignature) ;

    /*\\\*/

    /*/// "Pens" */

    get(data->colors[0], MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_QuoteLevel1) ;
    }

    get(data->colors[1], MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_QuoteLevel2) ;
    }

    get(data->colors[2], MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_QuoteLevel3) ;
    }

    get(data->colors[3], MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_QuoteLevel4) ;
    }

    get(data->colors[4], MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_QuoteLevel5) ;
    }

    get(data->urlcolor, MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_URL) ;
    }

    get(data->textcolor, MUIA_Pendisplay_Spec, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, sizeof(struct MUI_PenSpec), MUICFG_Mailtext_Color_Text) ;
    }

    /*\\\*/

    /*/// "Numeric values" */

    get(data->distObj, MUIA_Numeric_Value, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_Distance) ;
    }

    get(data->qdistObj, MUIA_Numeric_Value, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_QuoteDistance) ;
    }

    get(data->ustackObj, MUIA_Numeric_Value, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, &item, sizeof(LONG), MUICFG_Mailtext_URLActionStack) ;
    }


    /*\\\*/

    /*/// "Strings" */

    get(data->fontObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_Font) ;
    }

    get(data->disPatObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_DisablePattern) ;
    }

    get(data->enPatObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_EnablePattern) ;
    }

    get(data->ssPatObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_SigSepPattern) ;
    }

    get(data->niCharsObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_NonIntroducingChars) ;
    }

    get(data->termCharsObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_TerminatingChars) ;
    }

    get(data->wsCharsObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_WhitespaceChars) ;
    }

    get(data->qCharsObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_QuoteChars) ;
    }

    get(data->uactStrObj, MUIA_String_Contents, &item) ;
    if (item)
    {
        DoMethod(msg->configdata, MUIM_Dataspace_Add, item, strlen((STRPTR)item)+1, MUICFG_Mailtext_URLAction) ;
    }

    /*\\\*/

    return(0);
}

#ifndef __AROS__

/*\\\*/
/*/// "static ULONG SAVEDS_ASM Dispatcher(REG(d0) struct IClass *const cl, REG(a2) Object *const obj, REG(a1) const Msg msg)" */

static ULONG SAVEDS ASM Dispatcher(REG(a0, struct IClass *const cl), REG(a2, Object *const obj), REG(a1, const Msg msg))
{
    switch(msg->MethodID)
    {
        case OM_NEW                       : return(New    (cl,obj,(APTR)msg));
        case OM_DISPOSE                   : return(Dispose(cl,obj,(APTR)msg));
        case OM_GET                       : return(Get    (cl,obj,(APTR)msg));

        case MUIM_Settingsgroup_ConfigToGadgets: return(ConfigToGadgets(cl,obj,(APTR)msg));
        case MUIM_Settingsgroup_GadgetsToConfig: return(GadgetsToConfig(cl,obj,(APTR)msg));
    }

    return(DoSuperMethodA(cl,obj,msg));
}

/*\\\*/
/*/// "static BOOL ClassInitFunc(const struct Library *const base)" */

static BOOL ClassInitFunc(const struct Library *const base)
{

    LocaleBase = OpenLibrary("locale.library", 38L) ;
    UtilityBase = OpenLibrary("utility.library", 38L) ;

    if( LocaleBase && UtilityBase
    #if defined(__amigaos4__)
      && GETINTERFACE(ILocale, LocaleBase)
      && GETINTERFACE(IUtility, UtilityBase)
    #endif
    )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
}

/*\\\*/
/*/// "void ClassExitFunc(struct Library *base)" */

static VOID ClassExitFunc(const struct Library *const base)
{
    if (LocaleBase)
    {
    #if defined(__amigaos4__)
        DROPINTERFACE(ILocale);
    #endif
        CloseLibrary(LocaleBase) ;
    }

    if (UtilityBase)
    {
    #if defined(__amigaos4__)
        DROPINTERFACE(IUtility);
    #endif
        CloseLibrary(UtilityBase) ;
    }
}

/*\\\*/

#endif

