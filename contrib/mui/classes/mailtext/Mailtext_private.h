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

 Mailtext OpenSource project : http://sourceforge.net/projects/mailtext/

 $Id$

 Sourcecode for Mailtext.mcc

 MCC_Mailtext uses the following tags:

    0x805701xx

 Highest defined tagvalue:

 define MUICFG_Mailtext_Font         0x80570141

***************************************************************************/

/*
#define BETARELEASE "7"
*/
                                                                               /***/
/*** Include stuff ***/                                                        /***/
                                                                               /***/
#ifndef MAILTEXT_MCC_H                                                         /***/
#define MAILTEXT_MCC_H                                                         /***/
                                                                               /***/
#ifndef LIBRARIES_MUI_H                                                        /***/
#include "libraries/mui.h"                                                     /***/
#endif                                                                         /***/
                                                                               /***/
/*** MUI Defines ***/                                                          /***/
                                                                               /***/
#define MUIC_Mailtext  "Mailtext.mcc"                                          /***/
#define MUIC_MailtextP "Mailtext.mcp"
                                                                               /***/
#define MailtextObject MUI_NewObject(MUIC_Mailtext                             /***/
                                                                               /***/

/*** Pen-Defaults for MCP and MCC ***/

#define MAILTEXT_QUOTEPEN_DEFAULT MPEN_SHINE
#define MAILTEXT_TEXTPEN_DEFAULT MPEN_FILL

/*** Methods ***/

#define MUIM_Mailtext_CallAction            0x8057012d
#define MUIM_Mailtext_CopyToClip            0x8057013c                         /***/

/*** Method structs ***/

/*** Special method values ***/

/*** Special method flags ***/

/*** Attributes ***/

// obsolete:

// #define MUIA_Mailtext_BDistance             0x80570100
// #define MUIA_Mailtext_IDistance             0x80570102
// #define MUIA_Mailtext_UDistance             0x80570106
// #define MUIA_Mailtext_RDistance             0x8057010D

#define MUIA_Mailtext_Distance              0x80570101
#define MUIA_Mailtext_IncPercent            0x80570103  /* v10 [ISG] */        /***/
#define MUIA_Mailtext_ShowAttributes        0x80570104
#define MUIA_Mailtext_Text                  0x80570105  /* v10 [ISG] */        /***/
#define MUIA_Mailtext_QuoteChars            0x80570107  /* v10 [ISG] */        /***/
#define MUIA_Mailtext_QuoteDistance         0x80570108
#define MUIA_Mailtext_DisablePattern        0x80570109
#define MUIA_Mailtext_EnablePattern         0x8057010A
#define MUIA_Mailtext_TerminatingChars      0x8057010B
#define MUIA_Mailtext_NonIntroducingChars   0x8057010C
#define MUIA_Mailtext_WhitespaceChars       0x8057010E

#define MUIA_Mailtext_SeparateSignature     0x80570138
#define MUIA_Mailtext_SigSepPattern         0x80570140

#define MUIA_Mailtext_URLAction             0x8057012e
#define MUIA_Mailtext_URLActionBeep         0x80570134
#define MUIA_Mailtext_URLActionStack        0x8057012f
#define MUIA_Mailtext_URLUnderline          0x80570130

#define MUIA_Mailtext_Color_QuoteLevel1     0x8057011D
#define MUIA_Mailtext_Color_QuoteLevel2     0x8057011E
#define MUIA_Mailtext_Color_QuoteLevel3     0x8057011F
#define MUIA_Mailtext_Color_QuoteLevel4     0x80570120
#define MUIA_Mailtext_Color_QuoteLevel5     0x80570121

#define MUIA_Mailtext_Color_Text            0x80570122
#define MUIA_Mailtext_Color_URL             0x8057012c

#define MUIA_Mailtext_ForbidContextMenu     0x80570136  /* v18 [I..] */        /***/
#define MUIA_Mailtext_UseContextMenu        0x8057012a

#define MUIA_Mailtext_ActionEMail           0x8057013a  /* v19 [..G] */        /***/
#define MUIA_Mailtext_ActionURL             0x8057013b  /* v18 [..G] */        /***/
#define MUIA_Mailtext_DisplayRaw            0x80570139  /* v18 [.SG] */        /***/
#define MUIA_Mailtext_Wordwrap              0x8057013d  /* v18 [.SG] */        /***/

/*** Special attribute values ***/

/*** Structures, Flags & Values ***/

/*** Configs (private) ***/

#define MUICFG_Mailtext_DisablePattern      0x8057010F
#define MUICFG_Mailtext_Distance            0x80570110
#define MUICFG_Mailtext_EnablePattern       0x80570111
#define MUICFG_Mailtext_NonIntroducingChars 0x80570112
#define MUICFG_Mailtext_QuoteChars          0x80570113
#define MUICFG_Mailtext_QuoteDistance       0x80570114
#define MUICFG_Mailtext_ShowAttributes      0x80570115
#define MUICFG_Mailtext_TerminatingChars    0x80570116
#define MUICFG_Mailtext_WhitespaceChars     0x80570117

#define MUICFG_Mailtext_SeparateSignature   0x80570137
#define MUICFG_Mailtext_SigSepPattern       0x8057013f

#define MUICFG_Mailtext_URLAction           0x80570131
#define MUICFG_Mailtext_URLActionBeep       0x80570135
#define MUICFG_Mailtext_URLActionStack      0x80570132
#define MUICFG_Mailtext_URLUnderline        0x80570133

#define MUICFG_Mailtext_QuoteCharsUse       0x8057011C

#define MUICFG_Mailtext_Color_QuoteLevel1   0x80570123
#define MUICFG_Mailtext_Color_QuoteLevel2   0x80570124
#define MUICFG_Mailtext_Color_QuoteLevel3   0x80570125
#define MUICFG_Mailtext_Color_QuoteLevel4   0x80570126
#define MUICFG_Mailtext_Color_QuoteLevel5   0x80570127

#define MUICFG_Mailtext_Color_Text          0x80570128
#define MUICFG_Mailtext_Color_URL           0x8057012b

#define MUICFG_Mailtext_UseContextMenu      0x80570129
#define MUICFG_Mailtext_Wordwrap            0x8057013e
#define MUICFG_Mailtext_Font                0x80570141

/*** Defaults (private) ***/

#define MUID_Mailtext_DisablePattern         "(begin [0-9][0-9][0-9] #?|-----BEGIN PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----)"
#define MUID_Mailtext_Distance               50
#define MUID_Mailtext_EnablePattern          "(end|-----END PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----)"
#define MUID_Mailtext_IncPercent             20
#define MUID_Mailtext_QuoteChars             ">"
#define MUID_Mailtext_QuoteCharsUse          FALSE
#define MUID_Mailtext_QuoteDistance          15
#define MUID_Mailtext_SeparateSignature      FALSE
#define MUID_Mailtext_ShowAttributes         FALSE
#define MUID_Mailtext_SigSepPattern          "(--|-- |---|--- )"
#define MUID_Mailtext_Wordwrap               TRUE

  // URLActionStack is defined in kByte

#define MUID_Mailtext_URLActionStack         4
#define MUID_Mailtext_URLActionBeep          FALSE
#define MUID_Mailtext_URLUnderline           TRUE
#define MUID_Mailtext_DisplayRaw             FALSE
#define MUID_Mailtext_ForbidContextMenu      FALSE


  // MUID_Mailtext_URLChars is not configurable
  //
  // Characters described as 'xchars' in rfc1738,
  // with the exception of '~' and '#'.

#define MUID_Mailtext_URLChars               "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.+!*'(),;/?:@&=%~#"
#define MUID_Mailtext_EMailChars             "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@_-%."

  // The contextmenu is always set in mcc.New(),
  // regardless of this default. The default is
  // only used in the MCP. When changing this to
  // false, mcc.New() must be changed, too.

#define MUID_Mailtext_UseContextMenu         TRUE

#define MUID_Mailtext_NonIntroducingChars    " :.,^+\\|?([{&%$§\"!/*_#"
#define MUID_Mailtext_TerminatingChars       " )]}.,:+-'&\"!?*/_#"
#define MUID_Mailtext_WhitespaceChars        " ([{\"'&+*/_#"

// Debug fonctions
#ifdef MCCDEBUG
    #define D(a) a
    #ifdef __amigaos4__
        #define BUG DebugPrintF
    #else
        #define BUG kprintf
    #endif
#else
    #define D(a)
#endif // MCCDEBUG

// Compatability for broken GCC libamiga->SetSuperAttrs
#if defined(__amigaos4__) || defined(__AROS__)
    #define DoSuperSet SetSuperAttrs
#else
    ULONG __stdargs DoSuperSet(struct IClass *cl,Object *obj,ULONG tag1,...)
    {
        return DoSuperMethod(cl,obj,OM_SET,&tag1,NULL);
    }
#endif
                                                                               /***/
#endif /* MAILTEXT_MCC_H */                                                    /***/

/*/// "---Mailtext.mcc---" */

/****** Mailtext.mcc/---Mailtext.mcc *****************************************
*
*    ABSTRACT
*        Mailtext class is a subclass of NList.mcc. The NList package is
*        written and (C) by Gilles Masson.
*
*        Mailtext is especially designed to display electronic messages. Its
*        main features cover:
*
*        - handling of font attributes: *bold*, /italics/, _underline_ and
*            #coloured#. The text between the attribute characters '*', '/',
*            '_' and '#' will be printed with the repective style. To
*            accomplish this, Mailtext has a powerful analysis engine that
*            handles many special cases to obtain best results. Also,
*            processing can be switched off for certain passages of text,
*            i.e. uuencoded binaries, signatures, PGP messages.
*
*        - highlighting of quoted textpasses: Mailtext is able to display up
*            to five levels of quoted text in different colours. Quote are
*            recognised by specifying the quote characters that may be used
*            to introduce a quoted textpassage. The most common quote
*            character is '>'. be introduced by a given set of quote
*            characters, the most common is '>'.
*
*        - highlighting and handling of URLs: Uniform Resource Locators
*            (URLs) may be highlighted. Also, an AmigaDOS command can be
*            triggered whenever an URL is double clicked with the clicked
*            URL as an argument. By using an ARexx script you could handle
*            http:// URLs to your web, mailto: URLs back to your Mailer and
*            ftp:// URLs to your FTP client. Mailtext supports all URL types
*            specified in RFC 1738 "Uniform Resource Locators", Section 3.
*
*        As of NList, Mailtext also supports
*
*        - mark & copy to clipboard: use the mouse to mark text and have it
*            copied to the clipboard.
*
*        Mailtext v3.1 needs MUI v3.6 (muimaster.library v17) and the NList
*        package to work.
*
*    NOTE
*        To clear things up while speaking about attributes note the
*        following convention:
*
*        *Text*
*        ^    ^
*        |    the terminating char
*        the introducing char
*
*        So speaking of "the introducing char" means the character switching
*        on a certain textattribute while "the terminating char" means the
*        character that switches off a certain textattribute.
*
*    AUTHOR
*        Mailtext.mcc/Mailtext.mcp is written and © 1996-2001 by Olaf Peters
*        Mailtext.mcc/Mailtext.mcp is © 2006 by Mailtext Open Source Team
*
*    USAGE RESTRICTIONS
*        This program is free software; you can redistribute it and/or modify
*        it under the terms of the GNU General Public License as published by
*        the Free Software Foundation; either version 2 of the License, or
*        (at your option) any later version.
*
*        This program is distributed in the hope that it will be useful,
*        but WITHOUT ANY WARRANTY; without even the implied warranty of
*        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*        GNU General Public License for more details.
*
*        You should have received a copy of the GNU General Public License
*        along with this program; if not, write to the Free Software
*        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
*        02111-1307  USA.
*
*        Mailtext OpenSource project: http://sourceforge.net/projects/mailtext
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "---Programming Guidelines---" */

/****** Mailtext.mcc/---Programming Guidelines--- **************************************
*
*    GUIDELINES
*        Here are some guidelines that should be considered when using MCC
*        Mailtext in your own programs:
*
*        - use MUIA_NList_Font or MUIA_Font with i.e. MUIV_Font_Fixed when
*            initializing the Mailtext object, so you can be sure which kind
*            of font will be used by the Mailtext object.
*
*        - you should consider copying (parts of) Mailtext's contextmenu
*            into your application's menu. Here are the necessary
*            notifications:
*
*
*            Trigger "Copy To Clipboard":
*
*              DoMethod(<menuitem>, MUIM_Notify, MUIA_Menuitem_Trigger,
*                                   MUIV_EveryTime, <mailtextobject>, 1,
*                                   MUIM_Mailtext_CopyToClip) ;
*
*            Trigger "Display raw":
*
*              DoMethod(<menuitem>, MUIM_Notify, MUIA_Menuitem_Checked,
*                                   MUIV_EveryTime, <mailtextobject>, 3,
*                                   MUIM_Set, MUIA_Mailtext_DisplayRaw,
*                                             MUIV_TriggerValue) ;
*
*            Trigger "Wordwrap":
*
*              DoMethod(<menuitem>, MUIM_Notify, MUIA_Menuitem_Checked,
*                                   MUIV_EveryTime, <mailtextobject>, 3,
*                                   MUIM_Set, MUIA_Mailtext_Wordwrap,
*                                             MUIV_TriggerValue) ;
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_ActionEMail" */

/****** Mailtext.mcc/MUIA_Mailtext_ActionEMail *******************************
*
*    NAME
*        MUIA_Mailtext_ActionEMail -- (V19) [..G], STRPTR
*
*    FUNCTION
*        Whenever the user doubleclicks an eMail address (Usenet-style),
*        this attribute will be set to the clicked address string.
*
*        You can also set up a notification on this attribute.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_ActionURL" */

/****** Mailtext.mcc/MUIA_Mailtext_ActionURL *********************************
*
*    NAME
*        MUIA_Mailtext_ActionURL -- (V18) [..G], STRPTR
*
*    FUNCTION
*        Whenever the user doubleclicks an URL, this attribute will be set
*        to the clicked URL string.
*
*        You can also set up a notification on this attribute.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_DisplayRaw" */

/****** Mailtext.mcc/MUIA_Mailtext_DisplayRaw ********************************
*
*    NAME
*        MUIA_Mailtext_DisplayRaw -- (V18) [.SG], BOOL
*
*    FUNCTION
*        By setting this attribute to TRUE, you can disable Mailtext's
*        attribute engine: Mailtext will forward the text directly to
*        NFloattext, without processing it.
*
*        You can use this attribute to temporarily disable processing on the
*        user's request, i.e. by providing an appropriate menuitem:
*        Mailtext's context menuitem 'Display plain text' is directly
*        connected to this attribute, so you should mainly use it, if you
*        want to provide an own contextmenu and still offer the
*        functionality of Mailtext's internal menu.
*
*        Default value for MUIA_Mailtext_DisplayRaw is FALSE.
*
*    SEE ALSO
*        MUIA_Mailtext_ForbidContextMenu
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_IncPercent" */

/****** Mailtext.mcc/MUIA_Mailtext_IncPercent ********************************
*
*    NAME
*        MUIA_Mailtext_IncPercent -- (V10) [ISG], LONG
*
*    FUNCTION
*        Mailtext.mcc's internal textbuffer has to be larger than the
*        textlength of the text you set with MUIA_Mailtext_Text depending on
*        the number of attributes used in the text.
*
*        So Mailtext.mcc allocates a buffer that is x percent larger than
*        the textlength, x being the value of MUIA_Mailtext_IncPercent.
*
*        So if you think that the default of 20 percent is not sufficient
*        (or overkill) you may configure a different value.
*
*        If the formatted text does not fit into the private textbuffer, no
*        formatting will be done and the orgininal text will be displayed
*        instead.
*
*    NOTE
*        With Mailtext 3.0, the behaviour of this option changed a bit: the
*        buffer is now *always* at least 2 KB larger than the original text.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_Text" */

/****** Mailtext.mcc/MUIA_Mailtext_Text **************************************
*
*    NAME
*        MUIA_Mailtext_Text -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        String of characters to be displayed as messsagetext.
*
*        The text will be copied to a private buffer so you may free your
*        buffer after setting MUIA_Mailtext_Text.
*
*        Setting this attribute to NULL means clearing the current text.
*
*        When getting this attribute, you will just get back the pointer to
*        the textblock. You are NOT ALLOWED to change the text.
*
*    SEE ALSO
*        Floattext.mui/MUIA_Floattext_Text,
*        NFloattext.mcc/MUIA_NFloattext_Text
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_ForbidContextMenu" */

/****** Mailtext.mcc/MUIA_Mailtext_ForbidContextMenu *************************
*
*    NAME
*        MUIA_Mailtext_ForbidContextMenu -- (V18) [I..], BOOL
*
*    FUNCTION
*        Starting with Mailtext.mcc v18 (Release 3.0), every Mailtext object
*        can have its own contextmenu by default (configurable in
*        Mailtext.mcp).
*
*        If you want to use an own contextmenu for the Mailtext object in
*        your application, you must initialise the Mailtext object with
*        MUIA_Mailtext_ForbidContextMenu set to TRUE.
*
*        This will override the settings the user has made in Mailtext.mcp
*        and the Mailtext object will not try to create its own contextmenu.
*
*        Default value for MUIA_Mailtext_ForbidContextMenu is FALSE.
*
*    NOTE
*        If you set MUIA_Mailtext_ForbidContextMenu to TRUE, the checkmark
*        in Mailtext.mcp will simply do nothing, i.e. it can *not* be used
*        to switch *your* contextmenu.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_QuoteChars" */

/****** Mailtext.mcc/MUIA_Mailtext_QuoteChars ********************************
*
*    NAME
*        MUIA_Mailtext_QuoteChars -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        Set the characters that should be considered introducers of quoted
*        textpassages.
*
*        I.e. the string ">:" means lines having '>' or ':' in their first x
*        chars (x is configurable by using the distances page of
*        Mailtext.mcp) will be highlighted.
*
*        Default is ">", if you set MUIA_Mailtext_QuoteChars to NIL or "" no
*        highlighting will be done.
*
*        You may specify up to 8 quotechars. If you specify more, only the 8
*        leftmost chars of your string will be considered.
*
*        When getting this attributes, the pointer returned is READ ONLY.
*        You are not allowed to change the string!
*
*    NOTE
*        In Mailtext.mcp the user can decide to override an application's
*        quotechars by checking the Override checkmark in the Chars page and
*        setting her own quotechars. Any change to MUIA_Mailtext_QuoteChars
*        will have no effect then.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIA_Mailtext_Wordwrap" */

/****** Mailtext.mcc/MUIA_Mailtext_Wordwrap **********************************
*
*    NAME
*        MUIA_Mailtext_Wordwrap -- (V18) [.SG], BOOL
*
*    FUNCTION
*        When this attribute is set to FALSE, Mailtext will not wordwrap
*        long lines but display a horizontal scrollbar instead.
*
*        Note that this will *only* work, if the Mailtext object is
*        contained in an NListview object.
*
*        For compatibility reasons with Floattext.mui and Listview.mui,
*        the default value for MUIA_Mailtext_Wordwrap is TRUE.
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "MUIM_Mailtext_CopyToClip" */

/****** Mailtext.mcc/MUIM_Mailtext_CopyToClip *********************************
*
*    NAME
*        MUIM_Mailtext_CopyToClip
*
*    FUNCTION
*        Oops, still undocumented! Complain to olf!
*
*
******************************************************************************
*
*/

/*\\\*/
/*/// "Internal" */

/***ii* Mailtext.mcc/MUIA_Mailtext_BDistance *********************************
*
*    NAME
*        MUIA_Mailtext_BDistance -- (V10) [ISG], LONG
*
*    FUNCTION
*        Set the maximum number of chars between the introducing and
*        terminating char for the bold attribute ('*').
*
*        Default is 50 chars.
*
*    SEE ALSO
*        MUIA_Mailtext_Distance
*        MUIA_Mailtext_IDistance
*        MUIA_Mailtext_RDistance
*        MUIA_Mailtext_UDistance
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_Distance **********************************
*
*    NAME
*        MUIA_Mailtext_Distance -- (V10) [IS.], LONG
*
*    FUNCTION
*        The distance attribute(s) determine how many chars may be between
*        the introducing and terminating char of a textattribute.
*
*        Let's take a look at the following text:
*
*           *mailtext*
*
*        If the distance (or BDistance) is >= 8, 'mailtext' will be printed
*        boldface, if the distance is < 8 it will be printed normally.
*
*        You may configure different widths for bold, italics and underlined
*        with MUIA_Mailtext_BDistance, MUIA_Mailtext_IDistance and
*        MUIA_Mailtext_UDistance.
*
*        Default is 50 chars.
*
*    SEE ALSO
*        MUIA_Mailtext_BDistance
*        MUIA_Mailtext_IDistance
*        MUIA_Mailtext_RDistance
*        MUIA_Mailtext_UDistance
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_IDistance *********************************
*
*    NAME
*        MUIA_Mailtext_IDistance -- (V10) [ISG], LONG
*
*    FUNCTION
*        Set the maximum number of chars between the introducing and
*        terminating char for the italics attribute ('/').
*
*        Default is 50 chars.
*
*    SEE ALSO
*        MUIA_Mailtext_Distance
*        MUIA_Mailtext_BDistance
*        MUIA_Mailtext_RDistance
*        MUIA_Mailtext_UDistance
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_ShowAttributes ****************************
*
*    NAME
*        MUIA_Mailtext_ShowAttributes -- (V10) [ISG], BOOL
*
*    FUNCTION
*        If set to TRUE, the introducing and terminating chars will still be
*        displayed otherwise they won't.
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_UDistance *********************************
*
*    NAME
*        MUIA_Mailtext_UDistance -- (V10) [ISG], LONG
*
*    FUNCTION
*        Set the maximum number of chars between the introducing and
*        terminating char for the underline attribute ('_').
*
*        Default is 50 chars.
*
*    SEE ALSO
*        MUIA_Mailtext_Distance
*        MUIA_Mailtext_BDistance
*        MUIA_Mailtext_IDistance
*        MUIA_Mailtext_UDistance
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_QuoteDistance *****************************
*
*    NAME
*        MUIA_Mailtext_QuoteDistance -- (V10) [ISG], LONG
*
*    FUNCTION
*        This attributes determines how many chars in the beginning of a
*        line should be examined whether they contain a quotechar as
*        configured by MUIA_Mailtext_QuoteChars.
*
*        Default is 8 characters, if you set this to 0, no highlighting will
*        be done even if you configured quotechars.
*
*    SEE ALSO
*        MUIA_Mailtext_QuoteChars
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_DisablePattern ****************************
*
*    NAME
*        MUIA_Mailtext_DisablePattern -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        This attribute contains an AmigaDOS pattern.
*
*        If attribute parsing is enabled, each line of the text will be
*        matched against this pattern and if the line matches the pattern,
*        attribute parsing will be disabled for the following lines
*        including the current.
*
*        It will be enabled again, if a line matches the enable pattern (see
*        MUIA_Mailtext_EnablePattern).
*
*        Default is a big pattern consisting of the following parts (i.e. it
*        is concatenated by "(<part1>|<part2>)"):
*
*          "begin [0-9][0-9][0-9] #?"                    (uuencoded binaries)
*          "-----BEGIN PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----"
*                                                              (PGP messages)
*          "-- "                                                  (Signature)
*          "--- "                                                 (Signature)
*
*        I'm always looking for more blocks that may be hardcoded into the
*        class, so please mail me, if you find one.
*
*    SEE ALSO
*        MUIA_Mailtext_EnablePattern
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_EnablePattern *****************************
*
*    NAME
*        MUIA_Mailtext_EnablePattern -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        This attribute contains an AmigaDOS pattern.
*
*        If attribute parsing is disabled, each line of the text will be
*        matched against this pattern and if the line matches the pattern,
*        attribute parsing will be enabled again for the following lines
*        excluding (!) the current.
*
*        Default is a big pattern consisting of the following parts (i.e. it
*        is concatenated by "(<part1>|<part2>)"):
*
*          "end"                                           (uuencoded binary)
*          "-----END PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----"
*                                                               (PGP message)
*
*    SEE ALSO
*        MUIA_Mailtext_DisablePattern
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_TerminatingChars **************************
*
*    NAME
*        MUIA_Mailtext_TerminatingChars -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        This attribute determines which characters have to be next to a
*        terminating char to switch off the textattribute. So the
*        textatrribute will only be switched off, if the char next to the
*        terminating chars is contained in the given string or if the
*        terminating char is the last of the line.
*
*        Opposite to MUIA_Mailtext_NonIntroducingChars, the character in
*        front of the terminating char does not matter.
*
*        Default is [... to be defined ... ask olf!]
*
*    EXAMPLE
*        MUIA_Mailtext_TerminatingChars = ":"
*
*        In " *Text*passage " "Text" will not be printed boldface, but will
*        be in " *Text*: ".
*
*    SEE ALSO
*        MUIA_Mailtext_NonIntroducingChars
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_NonIntroducingChars ***********************
*
*    NAME
*        MUIA_Mailtext_NonIntroducingChars -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        This attribute determines which chars are not allowed to be the
*        character next to the introducing char. So the attribute will only
*        be switched on, if the character following the introducing char is
*        not contained in this string and if the chars in front of the
*        introducing char is a whitespace or the introducing char is the
*        first of the line.
*
*        Note that the switching of the textattribute also depends on
*        MUIA_Mailtext_TerminatingChars
*
*        Default is [... to be defined ... ask olf!]
*
*    EXAMPLE
*        MUIA_Mailtext_NonIntroducingChars = "-"
*
*        " *-Text-* " will not be printed boldface while " *Text* " will.
*
*    SEE ALSO
*        MUIA_Mailtext_TerminatingChars
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_RDistance *********************************
*
*    NAME
*        MUIA_Mailtext_RDistance -- (V10) [ISG], LONG
*
*    FUNCTION
*        Set the maximum number of chars between the introducing and
*        terminating char for the reverse attribute ('#').
*
*        As MUI does not support the reverse textstyle, the text will be
*        printed in a different color.
*
*        Default is 50 chars.
*
*    SEE ALSO
*        MUIA_Mailtext_Distance
*        MUIA_Mailtext_BDistance
*        MUIA_Mailtext_IDistance
*        MUIA_Mailtext_UDistance
*
*
******************************************************************************
*
*/

/***ii* Mailtext.mcc/MUIA_Mailtext_WhitespaceChars ***************************
*
*    NAME
*        MUIA_Mailtext_WhitespaceChars -- (V10) [ISG], STRPTR
*
*    FUNCTION
*        This attribute determines, which characters shall be considered
*        whitespace, i.e. being allowed in front of the introducing char.
*
*        Note that the QuoteChars configured with MUIA_Mailtext_QuoteChars
*        will also be considered whitespace, so you do not have to specify
*        them in the MUIA_Mailtext_WhitespaceChars as Mailtext.mcc will do
*        this for you.
*
*    SEE ALSO
*        MUIA_Mailtext_TerminatingChars
*        MUIA_Mailtext_NonIntroducingChars
*        MUIA_Mailtext_QuoteChars
*
******************************************************************************
*
*/

/*\\\*/

