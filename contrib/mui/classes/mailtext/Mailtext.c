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

 Sourcecode for Mailtext.mcc

***************************************************************************/

/* --- Includes --- */

#include <clib/alib_protos.h>

#if defined(__amigaos4__) || defined(__AROS__)
 #include <stdio.h>
#else
 #include <clib/alib_stdio_protos.h>
#endif /* __amigaos4__ */

#include <proto/exec.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <libraries/locale.h>
#include <string.h>
#include <dos/dostags.h>
#include <stdlib.h>

#include "Mailtext_private.h"
#include "DynStr.h"
#include "AttributesEngine.h"
#include "loc/Mailtext_mcc.h"
#include "URLs.h"

#ifdef __AROS__
#include <MUI/NList_mcc.h>
#else
#include <mui/NList_mcc.h>
#endif

#define CLASS         MUIC_Mailtext
#define SUPERCLASS    MUIC_NList
#define MUIMINVERSION 17

#ifdef __AROS__
#include "aros/Mailtext_mcc_private.h"
#else
struct Data
{
    STRPTR          ptext ;

    UWORD           percent ;

    char            urlmagic[16] ;

    LONG            urlmagiclen,
                    actionstack ;

    BOOL            qcOverride,
                    actionBeep,
                    cmForbid,
                    displayRaw,
                    wrap ;

    String          text,
                    urlaction,
                    url,
                    email,
                    fontName,
                    wschars ;

    LONG            tpen,
                    upen,
                    qpens[5] ;

    Object          *menu,
                    *miWrap,
                    *miRaw ;

    Bitfield        mailBF ;

    struct AEParams params ;
    struct TextFont *font ;     // Font, der in den Prefs eingestellt wurde.
    LONG            setFont ;   // Font, der per [IS.] gesetzt wurde.


    struct Catalog  *catalog ;
};
#endif

#ifdef __AROS__
#include "aros/MailtextVersionsAROS.h"
#else
#include "rev/Mailtext.mcc.h"
#endif

#ifndef __AROS__
#include <mui/mccbase.c>
#endif

struct Library *LocaleBase;
struct Library *DiskfontBase;

#if defined(__amigaos4__)
struct LocaleIFace   * ILocale   = NULL;
struct DiskfontIFace * IDiskfont = NULL;
#endif


/* --- Support-Functions --- */

/*/// "static void SetText(struct IClass *cl, Object *obj, struct Data *data, const String *text)" */

static void SetText(struct IClass *cl, Object *obj, struct Data *data, const String *text)
{
    ULONG increment ;
    ULONG textSize ;

    DoSuperMethod(cl, obj, MUIM_NList_Clear) ;

    if ((STR(*text)) && (*STR(*text)))
    {
        if (data->displayRaw)
        {
            DoSuperMethod(cl, obj, MUIM_NList_InsertWrap, STR(*text), -2, MUIV_NList_Insert_Top, (data->wrap ? WRAPCOL0 : NOWRAP), ALIGN_LEFT);
        }
        else
        {
            if ((textSize = (ULONG)strlen(STR(*text))) > 0)
            {
                if (data->ptext)
                {
                    FreeVec(data->ptext) ;
                    data->ptext = NULL ;
                }

                if ((increment = (textSize * data->percent) / 100) < 2048)
                {
                    increment = 2048 ;
                }

                textSize = textSize + increment ;

                data->ptext = (void*)AllocVec(textSize, MEMF_CLEAR) ;

                if (data->ptext && AE_SetAttributes(STR(*text), data->ptext, textSize, &data->params))
                {
                    DoSuperMethod(cl, obj, MUIM_NList_InsertWrap, data->ptext, -2, MUIV_NList_Insert_Top, (data->wrap ? WRAPCOL0 : NOWRAP), ALIGN_LEFT);
                }
            }
        }
    }
}

/*\\\*/
/*/// "static void InitPenData(struct AEAttrData *data, const LONG pen)" */

static void InitPenData(struct AEAttrData *data, const LONG pen)
{
    sprintf(data->set, "\033P[%ld]", pen) ;
    strcpy(data->unset, "\033P[]") ;

    data->setSize   = strlen(data->set) ;
    data->unsetSize = strlen(data->unset) ;
}

/*\\\*/
/*/// "static void SetupBitfield(Bitfield *bf, const char *bits)" */

static void SetupBitfield(Bitfield *bf, const char *bits)
{
    BF_ClearBitfield(bf) ;
    if (bits)
    {
        BF_FillBitfield(bf, bits) ;
    }
}

/*\\\*/
/*/// "static void SetupWSChars(Bitfield *bf, const char *wschars, const char *qchar" */

static void SetupWSChars(Bitfield *bf, const char *wschars, char *qchars)
{
    SetupBitfield(bf, wschars) ;
    BF_FillBitfield(bf, qchars) ;
    BF_Set(bf, '\t') ;
}

/*\\\*/
/*/// "static void MTReleasePen(struct MUI_RenderInfo *mri, LONG *pen)" */

static void MTReleasePen(struct MUI_RenderInfo *mri, LONG *pen)
{
    if(*pen != -1)
    {
        if(mri)
                MUI_ReleasePen(mri, *pen);

        *pen = -1;
    }
}

/*\\\*/
/*/// "static void MTObtainPen(struct MUI_RenderInfo *mri, LONG *pen, struct MUI_PenSpec *ps)" */

static void MTObtainPen(struct MUI_RenderInfo *mri, LONG *pen, struct MUI_PenSpec *ps)
{
    if(mri)
    {
        MTReleasePen(mri, pen);
        *pen = MUI_ObtainPen(mri, ps, 0);
    }
}

/*\\\*/

/*/// "WORD FindURLStart(char *entry, WORD pos, Data *data)" */

WORD FindURLStart(char *entry, WORD pos, struct Data *data)
{
    while (pos && BF_IsSet(&data->params.urlBF, entry[pos-1]))
    {
        pos-- ;
    }

    if (data->params.underlineURL)
    {
        if (!Strnicmp(data->urlmagic, &entry[pos], data->urlmagiclen))
        {
            pos += data->urlmagiclen ;
        }
    }

    return pos ;
}

/*\\\*/
/*/// "WORD FindMailStart(char *entry, WORD pos, struct Data *data)" */

WORD FindMailStart(char *entry, WORD pos, struct Data *data)
{
    while (pos && BF_IsSet(&data->mailBF, entry[pos-1]))
    {
        pos-- ;
    }

    return pos ;
}

/*\\\*/

/* --- Methods --- */

/*/// "static ULONG New(struct IClass *cl, Object *obj, struct opSet *msg)" */

#ifdef __AROS__
IPTR Mailtext__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
#else
static ULONG New(struct IClass *cl, Object *obj, struct opSet *msg)
#endif
{
    struct Data *data;
    STRPTR thetext ;
    Object *miCopy ;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)))
    {
        return(0);
    }

    data = INST_DATA(cl, obj);

    data->catalog = OpenMailtext_mccCatalog(NULL, "") ;

    data->cmForbid   = (BOOL)GetTagData(MUIA_Mailtext_ForbidContextMenu, MUID_Mailtext_ForbidContextMenu, msg->ops_AttrList) ;
    data->displayRaw = MUID_Mailtext_DisplayRaw ;
    data->wrap       = MUID_Mailtext_Wordwrap ;

    data->qcOverride = (BOOL)MUID_Mailtext_QuoteCharsUse ;

    if ((data->percent = (UWORD)GetTagData(MUIA_Mailtext_IncPercent, (ULONG)MUID_Mailtext_IncPercent, msg->ops_AttrList)) <= 0)
    {
        data->percent = (UWORD)MUID_Mailtext_IncPercent ;
    }

    if ((!data->cmForbid) && (data->menu = MenustripObject, Child, MenuObject, MUIA_Menu_Title, GetString(data->catalog, MSG_MenuTitle),
                                                                       Child, miCopy       = MUI_MakeObject(MUIO_Menuitem, GetString(data->catalog, MSG_MenuCopy),       0,0,0),
                                                                       Child, data->miRaw  = MUI_MakeObject(MUIO_Menuitem, GetString(data->catalog, MSG_MenuDisplayRaw), 0, CHECKIT|MENUTOGGLE, 0),
                                                                       Child, data->miWrap = MUI_MakeObject(MUIO_Menuitem, GetString(data->catalog, MSG_MenuWordwrap),  0, CHECKIT|MENUTOGGLE|CHECKED, 0),
                                                                   End,
                                           End))
    {
        set(obj, MUIA_ContextMenu, data->menu) ;

        DoMethod(miCopy, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj,
                                      1, MUIM_Mailtext_CopyToClip) ;

        DoMethod(data->miRaw,  MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime, obj,
                                            3, MUIM_Set, MUIA_Mailtext_DisplayRaw, MUIV_TriggerValue) ;

        DoMethod(data->miWrap, MUIM_Notify, MUIA_Menuitem_Checked, MUIV_EveryTime, obj,
                                            3, MUIM_Set, MUIA_Mailtext_Wordwrap, MUIV_TriggerValue) ;
    }

    DoSuperSet(cl, obj, MUIA_NList_AutoCopyToClip, FALSE,
               MUIA_NList_TypeSelect, MUIV_NList_TypeSelect_Char,
               TAG_DONE) ;

    data->params.bdata.dist  = MUID_Mailtext_Distance ;
    data->params.idata.dist  = MUID_Mailtext_Distance ;
    data->params.udata.dist  = MUID_Mailtext_Distance ;
    data->params.tcolor.dist = MUID_Mailtext_Distance ;
    data->params.qdist       = MUID_Mailtext_QuoteDistance ;

    strcpy(data->params.bdata.set,   MUIX_B) ;
    strcpy(data->params.bdata.unset, MUIX_N) ;
    data->params.bdata.setSize   = 2 ;
    data->params.bdata.unsetSize = 2 ;
    data->params.bdata.active    = FALSE ;

    strcpy(data->params.idata.set,   MUIX_I) ;
    strcpy(data->params.idata.unset, MUIX_N) ;
    data->params.idata.setSize   = 2 ;
    data->params.idata.unsetSize = 2 ;
    data->params.idata.active    = FALSE ;

    strcpy(data->params.udata.set,   MUIX_U) ;
    strcpy(data->params.udata.unset, MUIX_N) ;
    data->params.udata.setSize   = 2 ;
    data->params.udata.unsetSize = 2 ;
    data->params.udata.active    = FALSE ;

    strcpy(data->params.tcolor.set,   "\0335") ;
    strcpy(data->params.tcolor.unset, MUIX_PT) ;
    data->params.tcolor.setSize   = 2 ;
    data->params.tcolor.unsetSize = 2 ;
    data->params.tcolor.active    = FALSE ;

    strcpy(data->params.ucolor.set,   "\0335") ;
    strcpy(data->params.ucolor.unset, MUIX_PT) ;
    data->params.ucolor.setSize = 2 ;
    data->params.ucolor.unsetSize = 2 ;
    data->params.ucolor.active = FALSE ;

    data->tpen     = -1 ;
    data->upen     = -1 ;
    data->qpens[0] = data->qpens[1] = data->qpens[2] = data->qpens[3] = data->qpens[4] = -1 ;

    strcpy(data->params.sigSeparator, "\033C") ;
    data->params.separateSig = MUID_Mailtext_SeparateSignature ;

    strncpy(data->urlmagic, "u", sizeof(data->urlmagic)-1) ; // leftmost URL-characters in MUIX_U
    data->urlmagiclen = 1 ;

    data->params.underlineURL = MUID_Mailtext_URLUnderline ;
    data->actionBeep = MUID_Mailtext_URLActionBeep ;

    INIT(data->urlaction) ;
    INIT(data->url) ;
    INIT(data->email) ;

    INIT(data->fontName) ;
    data->font = NULL ;
    data->setFont = GetTagData(MUIA_Font, MUIV_Font_Inherit, msg->ops_AttrList) ;

    DupString(&data->params.epattern, MUID_Mailtext_EnablePattern) ;
    DupString(&data->params.dpattern, MUID_Mailtext_DisablePattern) ;
    DupString(&data->params.spattern, MUID_Mailtext_SigSepPattern) ;
    DupString(&data->wschars,         MUID_Mailtext_WhitespaceChars) ;

    strncpy(data->params.qchars, (STRPTR)GetTagData(MUIA_Mailtext_QuoteChars, (ULONG)MUID_Mailtext_QuoteChars, msg->ops_AttrList), sizeof(data->params.qchars)-1) ;

    SetupBitfield(&data->params.scharsBF,  MUID_Mailtext_NonIntroducingChars) ;
    SetupBitfield(&data->params.tcharsBF,  MUID_Mailtext_TerminatingChars) ;
    SetupBitfield(&data->params.qcharsBF,  data->params.qchars) ;
    SetupBitfield(&data->params.attsBF,    "*/_#") ;
    SetupBitfield(&data->params.urlBF,     MUID_Mailtext_URLChars) ;
    SetupBitfield(&data->mailBF,           MUID_Mailtext_EMailChars) ;

    SetupWSChars(&data->params.wscharsBF, MUID_Mailtext_WhitespaceChars, data->params.qchars) ;

    DoMethod(obj, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime, obj,
                  1, MUIM_Mailtext_CallAction) ;

    data->params.showAttrs = MUID_Mailtext_ShowAttributes ;

    if (thetext = (STRPTR)GetTagData(MUIA_Mailtext_Text, (ULONG)"", msg->ops_AttrList))
    {
        DupString(&data->text, thetext) ;
    }
    else
    {
        INIT(data->text) ;
    }

    data->ptext = NULL ;

    return((ULONG)obj);
}

/*\\\*/
/*/// "static ULONG Dispose(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR Mailtext__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG Dispose(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj) ;

    CloseMailtext_mccCatalog(data->catalog) ;

    if (data->menu)
    {
        MUI_DisposeObject(data->menu) ;
        data->menu = NULL ;
    }

    if (data->font)
    {
        CloseFont(data->font) ;
        data->font = NULL ;
    }

    FreeString(&data->fontName) ;
    FreeString(&data->urlaction) ;
    FreeString(&data->url) ;
    FreeString(&data->email) ;
    FreeString(&data->text) ;
    FreeString(&data->wschars) ;

    FreeString(&data->params.epattern) ;
    FreeString(&data->params.dpattern) ;

    if (data->ptext)
    {
        FreeVec(data->ptext) ;
        data->ptext = NULL ;
    }

    return(DoSuperMethodA(cl,obj,msg));
}

/*\\\*/

/*/// "static ULONG Set(struct IClass *cl, Object *obj, struct opSet *msg)" */

#if __AROS__
IPTR Mailtext__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
#else
static ULONG Set(struct IClass *cl, Object *obj, struct opSet *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    struct TagItem *tags,*tag;
    LONG first = MUIV_NList_First_Top ;
    BOOL changed = FALSE, textchanged = FALSE ;

    get(obj, MUIA_NList_First, &first) ;

    for(tags=msg->ops_AttrList; tag=NextTagItem(&tags); )
    {
        switch(tag->ti_Tag)
        {
            case MUIA_Font :

                if (tag->ti_Data != (ULONG)data->font)
                {
                    data->setFont = tag->ti_Data ;
                }

            break ;

            case MUIA_Mailtext_Text :

                if (tag->ti_Data)
                {
                    DupString(&data->text, (STRPTR)tag->ti_Data) ;
                }
                else
                {
                    FreeString(&data->text) ;
                }
                changed = TRUE ;
                textchanged = TRUE ;

            break ;

            case MUIA_Mailtext_Color_QuoteLevel1 :

                MTObtainPen(muiRenderInfo(obj), &data->qpens[0], (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.qcolors[0], data->qpens[0]) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_QuoteLevel2 :

                MTObtainPen(muiRenderInfo(obj), &data->qpens[1], (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.qcolors[1], data->qpens[1]) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_QuoteLevel3 :

                MTObtainPen(muiRenderInfo(obj), &data->qpens[2], (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.qcolors[2], data->qpens[2]) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_QuoteLevel4 :

                MTObtainPen(muiRenderInfo(obj), &data->qpens[3], (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.qcolors[3], data->qpens[3]) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_QuoteLevel5 :

                MTObtainPen(muiRenderInfo(obj), &data->qpens[4], (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.qcolors[4], data->qpens[4]) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_URL :

                MTObtainPen(muiRenderInfo(obj), &data->upen, (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.ucolor, data->upen) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Color_Text :

                MTObtainPen(muiRenderInfo(obj), &data->tpen, (struct MUI_PenSpec *)tag->ti_Data) ;
                InitPenData(&data->params.tcolor, data->tpen) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_DisablePattern :

                DupString(&data->params.dpattern, (STRPTR)tag->ti_Data) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_DisplayRaw :
                data->displayRaw = (BOOL)tag->ti_Data ;

                if (!data->cmForbid)
                {
                    nnset(data->miRaw, MUIA_Menuitem_Checked, data->displayRaw) ;
                }

                changed = TRUE ;
            break ;

            case MUIA_Mailtext_Distance :

                data->params.bdata.dist  = (UWORD)tag->ti_Data ;
                data->params.idata.dist  = (UWORD)tag->ti_Data ;
                data->params.udata.dist  = (UWORD)tag->ti_Data ;
                data->params.tcolor.dist = (UWORD)tag->ti_Data ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_EnablePattern :

                DupString(&data->params.epattern, (STRPTR)tag->ti_Data) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_SigSepPattern :

                DupString(&data->params.spattern, (STRPTR)tag->ti_Data) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_IncPercent :

                if (tag->ti_Data > 0)
                {
                    data->percent = (UWORD)tag->ti_Data ;
                }
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_NonIntroducingChars :

                SetupBitfield(&data->params.scharsBF, (STRPTR)tag->ti_Data) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_QuoteChars :
                if (!data->qcOverride)
                {
                    strncpy(data->params.qchars, (STRPTR)tag->ti_Data, sizeof(data->params.qchars)-1) ;
                    SetupBitfield(&data->params.qcharsBF, data->params.qchars) ;

                    SetupWSChars(&data->params.wscharsBF, STR(data->wschars), data->params.qchars) ;
                }
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_QuoteDistance :

                data->params.qdist = (UWORD)tag->ti_Data ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_SeparateSignature :

                data->params.separateSig = (BOOL)tag->ti_Data ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_ShowAttributes :

                data->params.showAttrs = (BOOL)tag->ti_Data ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_URLAction :

                DupString(&data->urlaction, (STRPTR)tag->ti_Data) ;

            break ;

            case MUIA_Mailtext_URLActionStack :

                data->actionstack = (LONG)tag->ti_Data ;

            break ;

            case MUIA_Mailtext_URLActionBeep :

                data->actionBeep = (BOOL)tag->ti_Data ;

            break ;

            case MUIA_Mailtext_URLUnderline :

                data->params.underlineURL = (BOOL)tag->ti_Data ;
                changed = TRUE ;
            break ;

            case MUIA_Mailtext_UseContextMenu :

                if (!data->cmForbid)
                {
                    if ((BOOL)tag->ti_Data)
                    {
                        set(obj, MUIA_ContextMenu, data->menu) ;
                    }
                    else
                    {
                        set(obj, MUIA_ContextMenu, NULL) ;
                    }
                }

            break ;

            case MUIA_Mailtext_TerminatingChars :

                SetupBitfield(&data->params.tcharsBF, (STRPTR)tag->ti_Data) ;
                changed = TRUE ;

            break ;

            case MUIA_Mailtext_Wordwrap :

                data->wrap = (BOOL)tag->ti_Data ;

                if (!data->cmForbid)
                {
                    nnset(data->miWrap, MUIA_Menuitem_Checked, data->wrap) ;
                }

                changed = TRUE ;

            break ;

            case MUIA_Mailtext_WhitespaceChars :

                DupString(&data->wschars, (STRPTR)tag->ti_Data) ;
                SetupWSChars(&data->params.wscharsBF, STR(data->wschars), data->params.qchars) ;
                changed = TRUE ;

            break ;
        }
    }

    if (changed)
    {
        D(BUG("Mailtext.mcc/Set: changed == TRUE\n"));

        DoSuperSet(cl, obj, MUIA_NList_Quiet, TRUE, TAG_DONE) ;

        SetText(cl, obj, data, &data->text) ;

        if (!textchanged)
        {
            DoSuperSet(cl, obj, MUIA_NList_First, first, TAG_DONE) ;
        }

        DoSuperSet(cl, obj, MUIA_NList_Quiet, FALSE, TAG_DONE) ;
    }

    return(DoSuperMethodA(cl, obj, (Msg)msg));
}

/*\\\*/
/*/// "static ULONG Get(struct IClass *cl, Object *obj, struct opGet *msg)" */

#if __AROS__
IPTR Mailtext__OM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
#else
static ULONG Get(struct IClass *cl, Object *obj, struct opGet *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    ULONG *store = msg->opg_Storage;

    switch( msg->opg_AttrID )
    {
        case MUIA_Version :                 *store = VERSION ;                          return (TRUE) ; break ;
        case MUIA_Revision :                *store = REVISION ;                         return (TRUE) ; break ;

        case MUIA_Mailtext_ActionEMail :    *store = (ULONG)STR(data->email) ;          return (TRUE) ; break ;
        case MUIA_Mailtext_ActionURL :      *store = (ULONG)STR(data->url) ;            return (TRUE) ; break ;
        case MUIA_Mailtext_IncPercent :     *store = (ULONG)data->percent ;             return (TRUE) ; break ;
        case MUIA_Mailtext_QuoteChars :     *store = (ULONG)&data->params.qchars[0];    return (TRUE) ; break ;
        case MUIA_Mailtext_Text :           *store = (ULONG)STR(data->text) ;           return (TRUE) ; break ;
        case MUIA_Mailtext_DisplayRaw :     *store = (ULONG)data->displayRaw ;          return (TRUE) ; break ;
        case MUIA_Mailtext_Wordwrap :       *store = (ULONG)data->wrap ;                return (TRUE) ; break ;
    }

    return(DoSuperMethodA(cl, obj, (Msg)msg));
}

/*\\\*/

/*/// "static ULONG Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)" */

#ifdef __AROS__
IPTR Mailtext__MUIM_Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
#else
static ULONG Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    LONG *item ;
    Object *pendisplay ;

    D(BUG("Mailtext.mcc/Setup\n"));

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Font, &item) && item)
    {
        struct TextAttr ta = {"", 0, 0, 0} ;
        char *c ;
        ULONG l ;
        char fnt[54] ;

        if (strlen((STRPTR)item))
        {
            if (data->font)
            {
                CloseFont(data->font) ;
                data->font = NULL ;
            }

            c = strrchr((STRPTR)item, '/') ;

            fnt[0] = '\0' ;

            l = (ULONG)c - (ULONG)item ;

            strncpy(fnt, (STRPTR)item, l) ;
            fnt[l] = '\0' ;
            strcat(fnt, ".font") ;

            DupString(&data->fontName, fnt) ;
            ta.ta_Name = STR(data->fontName) ;
            ta.ta_YSize = atoi(++c) ;


            if (data->font = OpenDiskFont(&ta))
            {
                D(BUG("Font: %s (%ld)\n", ta.ta_Name, ta.ta_YSize)) ;
                DoSuperSet(cl, obj, MUIA_Font, data->font, TAG_DONE) ;
            }
            else
            {
                DoSuperSet(cl, obj, MUIA_Font, data->setFont, TAG_DONE) ;
            }
        }
        else
        {
            DoSuperSet(cl, obj, MUIA_Font, data->setFont, TAG_DONE) ;
        }
    }
    else
    {
        DoSuperSet(cl, obj, MUIA_Font, data->setFont, TAG_DONE) ;
    }

    D(BUG("data->setFont = %lx\n", data->setFont)) ;

    if(!(DoSuperMethodA(cl,obj,(Msg)msg)))
    {
        return(FALSE);
    }

    pendisplay = PendisplayObject, End ;

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_ShowAttributes, &item) && item)
    {
        data->params.showAttrs = (BOOL)*item ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_SeparateSignature, &item) && item)
    {
        data->params.separateSig = (BOOL)*item ;
    }


    if (!data->cmForbid)
    {
        if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_UseContextMenu, &item) && item)
        {
            if ((BOOL)*item)
            {
                set(obj, MUIA_ContextMenu, data->menu) ;
            }
            else
            {
                set(obj, MUIA_ContextMenu, NULL) ;
            }
        }
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Distance, &item) && item)
    {
        data->params.bdata.dist  = *item ;
        data->params.idata.dist  = *item ;
        data->params.udata.dist  = *item ;
        data->params.tcolor.dist = *item ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_QuoteDistance, &item) && item)
    {
        data->params.qdist = *item ;
    }

    /*/// "ColorURL" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_URL, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->upen, (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.ucolor, data->upen) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_TEXTPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->upen, pen) ;

            InitPenData(&data->params.ucolor, data->upen) ;
        }
        else
        {
            data->upen = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorText" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_Text, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->tpen, (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.tcolor, data->tpen) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_TEXTPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->tpen, pen) ;

            InitPenData(&data->params.tcolor, data->tpen) ;
        }
        else
        {
            data->tpen = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorQuoteLevel1" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_QuoteLevel1, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->qpens[0], (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.qcolors[0], data->qpens[0]) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->qpens[0], pen) ;

            InitPenData(&data->params.qcolors[0], data->qpens[0]) ;
        }
        else
        {
            data->qpens[0] = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorQuoteLevel2" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_QuoteLevel2, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->qpens[1], (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.qcolors[1], data->qpens[1]) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->qpens[1], pen) ;

            InitPenData(&data->params.qcolors[1], data->qpens[1]) ;
        }
        else
        {
            data->qpens[1] = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorQuoteLevel3" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_QuoteLevel3, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->qpens[2], (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.qcolors[2], data->qpens[2]) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->qpens[2], pen) ;

            InitPenData(&data->params.qcolors[2], data->qpens[2]) ;
        }
        else
        {
            data->qpens[2] = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorQuoteLevel4" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_QuoteLevel4, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->qpens[3], (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.qcolors[3], data->qpens[3]) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->qpens[3], pen) ;

            InitPenData(&data->params.qcolors[3], data->qpens[3]) ;
        }
        else
        {
            data->qpens[3] = -1 ;
        }
    }

    /*\\\*/
    /*/// "ColorQuoteLevel5" */

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Color_QuoteLevel5, &item) && item)
    {
        MTObtainPen(muiRenderInfo(obj), &data->qpens[4], (struct MUI_PenSpec *)item) ;

        InitPenData(&data->params.qcolors[4], data->qpens[4]) ;
    }
    else
    {
        if (pendisplay)
        {
            struct MUI_PenSpec *pen ;

            DoMethod(pendisplay, MUIM_Pendisplay_SetMUIPen, MAILTEXT_QUOTEPEN_DEFAULT) ;
            get(pendisplay, MUIA_Pendisplay_Spec, &pen) ;
            MTObtainPen(muiRenderInfo(obj), &data->qpens[4], pen) ;

            InitPenData(&data->params.qcolors[4], data->qpens[4]) ;
        }
        else
        {
            data->qpens[4] = -1 ;
        }
    }

    /*\\\*/

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_DisablePattern, &item) && item)
    {
        DupString(&data->params.dpattern, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_EnablePattern, &item) && item)
    {
        DupString(&data->params.epattern, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_SigSepPattern, &item) && item)
    {
        DupString(&data->params.spattern, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_NonIntroducingChars, &item) && item)
    {
        SetupBitfield(&data->params.scharsBF, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_WhitespaceChars, &item) && item)
    {
        DupString(&data->wschars, (STRPTR)item) ;
        SetupWSChars(&data->params.wscharsBF, STR(data->wschars), data->params.qchars) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_TerminatingChars, &item) && item)
    {
        SetupBitfield(&data->params.tcharsBF, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_QuoteCharsUse, &item) && item)
    {
        data->qcOverride = (BOOL)*item ;

        if (data->qcOverride && DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_QuoteChars, &item) && item)
        {
            strncpy(data->params.qchars, (STRPTR)item, sizeof(data->params.qchars)-1) ;
            SetupBitfield(&data->params.qcharsBF, data->params.qchars) ;

            SetupWSChars(&data->params.wscharsBF, STR(data->wschars), data->params.qchars) ;
        }
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_URLAction, &item) && item)
    {
        DupString(&data->urlaction, (STRPTR)item) ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_URLActionStack, &item) && item)
    {
        data->actionstack = *item ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_URLActionBeep, &item) && item)
    {
        data->actionBeep = (BOOL)*item ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_URLUnderline, &item) && item)
    {
        data->params.underlineURL = (BOOL)*item ;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_Mailtext_Wordwrap, &item) && item)
    {
        data->wrap = (BOOL)*item ;

        if (!data->cmForbid)
        {
            nnset(data->miWrap, MUIA_Menuitem_Checked, data->wrap) ;
        }
    }

    if (pendisplay)
    {
        MUI_DisposeObject(pendisplay) ;
    }

    SetText(cl, obj, data, &data->text) ;

    return(TRUE);
}

/*\\\*/
/*/// "static ULONG Cleanup(struct IClass *cl, Object *obj, struct MUIP_Cleanup *msg)" */

#ifdef __AROS__
IPTR Mailtext__MUIM_Cleanup(struct IClass *cl, Object *obj, struct MUIP_Cleanup *msg)
#else
static ULONG Cleanup(struct IClass *cl, Object *obj, struct MUIP_Cleanup *msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);

    D(BUG("Mailtext.mcc/Cleanup\n"));

    MTReleasePen(muiRenderInfo(obj), &data->tpen) ;
    MTReleasePen(muiRenderInfo(obj), &data->upen) ;
    MTReleasePen(muiRenderInfo(obj), &data->qpens[0]) ;
    MTReleasePen(muiRenderInfo(obj), &data->qpens[1]) ;
    MTReleasePen(muiRenderInfo(obj), &data->qpens[2]) ;
    MTReleasePen(muiRenderInfo(obj), &data->qpens[3]) ;
    MTReleasePen(muiRenderInfo(obj), &data->qpens[4]) ;

    if(!(DoSuperMethodA(cl,obj,(Msg)msg)))
    {
        return(FALSE);
    }

    return(TRUE);
}

/*\\\*/

/*/// "static ULONG CopyToClip(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR Mailtext__MUIM_Mailtext_CopyToClip(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG CopyToClip(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    DoSuperMethod(cl, obj, MUIM_NList_CopyToClip, MUIV_NList_CopyToClip_Selected, 0, NULL, NULL, TAG_DONE) ;
    DoSuperMethod(cl, obj, MUIM_NList_Select, 0, MUIV_NList_Select_Off, NULL, TAG_DONE) ;

    return (ULONG)(NULL) ;
}

/*\\\*/
/*/// "static ULONG CallAction(struct IClass *cl, Object *obj, Msg msg)" */

#ifdef __AROS__
IPTR Mailtext__MUIM_Mailtext_CallAction(struct IClass *cl, Object *obj, Msg msg)
#else
static ULONG CallAction(struct IClass *cl, Object *obj, Msg msg)
#endif
{
    struct Data *data = INST_DATA(cl, obj);
    struct MUI_NList_TestPos_Result res ;
    char *entry ;

    if (!(STR(data->urlaction) && STR(data->urlaction)[0]))
    {
        return (ULONG)(NULL) ;
    }

    DoMethod(obj, MUIM_NList_TestPos, MUI_MAXMAX, MUI_MAXMAX, &res) ;

    if ((res.entry != -1) && (res.char_number != -1))
    {
        WORD urlstart, mailstart ;
        BOOL testEMail = TRUE ;

        DoMethod(obj, MUIM_NList_GetEntry, res.entry, &entry) ;

        if (!(*entry))
        {
            return (ULONG)(NULL) ;
        }

        urlstart = FindURLStart(entry, res.char_number, data) ;
        mailstart = FindMailStart(entry, res.char_number, data) ;

/*/// "Trigger URL-Action" */

        if (IsURL(&entry[urlstart], &data->params.urlBF))
        {
            String cmd ;
            LONG ustart, ulength ;
            BPTR In = Open("NIL:", MODE_OLDFILE) ;
            BPTR Out = Open("NIL:", MODE_NEWFILE) ;

            ustart = urlstart ;

            if (!Strnicmp("URL:", &entry[ustart], 4))
            {
                ustart += 4 ;
            }

            while (    BF_IsSet(&data->params.urlBF, entry[urlstart])
                   && (    BF_IsSet(&data->params.urlBF, entry[urlstart+1])
                       || (entry[urlstart] == '/')
                       || (!BF_IsSet(&data->params.tcharsBF, entry[urlstart]))))
            {
                urlstart++ ;
            }

            if (!Strnicmp("mailto:", &entry[ustart], 7))
            {
                ustart += 7 ;

                DupStringN(&data->email, &entry[ustart], urlstart - ustart) ;

                set(obj, MUIA_Mailtext_ActionEMail, STR(data->email)) ;

                testEMail = FALSE ;
            }
            else
            {
                ulength = urlstart - ustart ;

                DupStringN(&data->url, &entry[ustart], ulength) ;

                AllocString(&cmd, strlen(STR(data->urlaction)) + ulength) ;

                sprintf(STR(cmd), STR(data->urlaction), STR(data->url)) ;

                // In and Out are closed by SystemTagList as SYS_Asynch is used, unless there's an error.

                if (SystemTags(STR(cmd), NP_StackSize, data->actionstack << 10,
                                         SYS_Input,    In,
                                         SYS_Output,   Out,
                                         SYS_Asynch,   TRUE))
                {
                    // Error, we have to close In and Out ourselves.

                    Close(In) ;
                    Close(Out) ;
                }
                else
                {
                    // No error doesn't necessarily mean success, but we'll beep anyway. :-)

                    if (data->actionBeep)
                    {
                        DisplayBeep(NULL) ;
                    }
                }

                FreeString(&cmd) ;

                // send out the notification

                set(obj, MUIA_Mailtext_ActionURL, STR(data->url)) ;
            }
        }

/*\\\*/

        if (IsEMail(&entry[mailstart], &data->mailBF) && testEMail)
        {
            LONG estart, elength ;

            estart = mailstart ;

            while (BF_IsSet(&data->mailBF, entry[mailstart]))
            {
                mailstart++ ;
            }

            if (entry[mailstart-1] == '.')
            {
                mailstart-- ;
            }

            elength = mailstart - estart ;

            DupStringN(&data->email, &entry[estart], elength) ;

            set(obj, MUIA_Mailtext_ActionEMail, STR(data->email)) ;
        }
    }

    return (ULONG)(NULL) ;
}

#ifndef __AROS__

/*\\\*/
/*/// "static ULONG SAVEDS_ASM Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)" */

static ULONG SAVEDS ASM Dispatcher(REG(a0, struct IClass *const cl), REG(a2, Object *const obj), REG(a1, const Msg msg))
{

 #ifdef MCCDEBUG
     if ((msg->MethodID != 0x9d510090) && (msg->MethodID != MUIM_HandleInput))
        D(BUG("Mailtext.mcc/Dispatcher: Method %lx\n", msg->MethodID));
 #endif

    switch(msg->MethodID)
    {
        case OM_NEW        : return(New    (cl, obj, (APTR)msg)) ;
        case OM_DISPOSE    : return(Dispose(cl, obj, (APTR)msg)) ;
        case OM_SET        : return(Set    (cl, obj, (APTR)msg)) ;
        case OM_GET        : return(Get    (cl, obj, (APTR)msg)) ;

        case MUIM_Setup    : return(Setup  (cl, obj, (APTR)msg)) ;
        case MUIM_Cleanup  : return(Cleanup(cl, obj, (APTR)msg)) ;

//      case MUIM_Show     : return(Show   (cl, obj, (APTR)msg)) ;
//      case MUIM_Hide     : return(Hide   (cl, obj, (APTR)msg)) ;

        case MUIM_Mailtext_CallAction : return(CallAction(cl, obj, (APTR)msg)) ;
        case MUIM_Mailtext_CopyToClip : return(CopyToClip(cl, obj, (APTR)msg)) ;
    }

    return(DoSuperMethodA(cl,obj,msg));
}

/*\\\*/
/*/// "static BOOL ClassInitFunc(const struct Library *const base)" */

static BOOL ClassInitFunc(const struct Library *const base)
{
    D(BUG( "ENTER %s\n",__FUNCTION__ ));

    LocaleBase = OpenLibrary("locale.library", 38L) ;
    DiskfontBase = OpenLibrary("diskfont.library", 38L) ;

    if( LocaleBase && DiskfontBase
    #if defined(__amigaos4__)
      && GETINTERFACE(ILocale, LocaleBase)
      && GETINTERFACE(IDiskfont, DiskfontBase)
    #endif
    )
    {
      D(BUG( "EXIT WITH TRUE %s\n",__FUNCTION__ ));
      return TRUE;
    }
    else
    {
      D(BUG( "EXIT WITH FALSE %s\n",__FUNCTION__ ));
      return FALSE;
    }
}

/*\\\*/
/*/// "static VOID ClassExitFunc(const struct Library *const base)" */

static VOID ClassExitFunc(const struct Library *const base)
{
    if (LocaleBase)
    {
    #if defined(__amigaos4__)
        DROPINTERFACE(ILocale);
    #endif
        CloseLibrary(LocaleBase) ;
    }

    if (DiskfontBase)
    {
    #if defined(__amigaos4__)
        DROPINTERFACE(IDiskfont);
    #endif
        CloseLibrary(DiskfontBase) ;
    }

}

/*\\\*/

#endif
