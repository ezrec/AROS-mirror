/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"
#include "private.h"
#include "rev.h"

#include <mui/muiundoc.h>

#include "TheBar_mcp.h"

#include "SDI_hook.h"
#include "Debug.h"

#undef GetOutlinePen
#include <graphics/gfxmacros.h>

/***********************************************************************/

#ifdef __MORPHOS__
ULONG WritePixelArrayAlpha(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD, UWORD, UWORD, UWORD, ULONG);
#define WritePixelArrayAlpha(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) \
	LP10(216, ULONG , WritePixelArrayAlpha, \
		APTR ,             __p0, a0, \
		UWORD ,            __p1, d0, \
		UWORD ,            __p2, d1, \
		UWORD , 	         __p3, d2, \
		struct RastPort *, __p4, a1, \
		UWORD , 	         __p5, d3, \
		UWORD , 	         __p6, d4, \
		UWORD ,            __p7, d5, \
		UWORD ,            __p8, d6, \
		ULONG ,            __p9, d7, \
		, CyberGfxBase, 0, 0, 0, 0, 0, 0)
#elif defined(__amigaos4__)
#define WritePixelArrayAlpha(srcRect, SrcX, SrcY, SrcMod, RastPort, DestX, DestY, SizeX, SizeY, globalAlpha) ICyberGfx->WritePixelArrayAlpha(srcRect, SrcX, SrcY, SrcMod, RastPort, DestX, DestY, SizeX, SizeY, globalAlpha)
#endif

/***********************************************************************/

struct pack
{
    Object                   *tb;
    ULONG                    id;
    struct MUIS_TheBar_Brush *image;
    struct MUIS_TheBar_Brush *simage;
    struct MUIS_TheBar_Brush *dimage;
    STRPTR                   label;
    ULONG                    vMode;
    ULONG                    lPos;
    ULONG                    flags;
    ULONG                    flags2;
    ULONG                    userFlags;
    ULONG                    nflags;

    LONG                     horizTextGfxSpacing;
    LONG                     vertTextGfxSpacing;
    LONG                     horizInnerSpacing;
    LONG                     topInnerSpacing;
    LONG                     bottomInnerSpacing;

    ULONG                    precision;
    ULONG                    disMode;
    ULONG                    scale;

    struct MUIS_TheBar_Strip *strip;
};

enum
{
    NFLG_Toggle     = 1<<0,
    NFLG_Immediate  = 1<<1,
};

ULONG packTable[] =
{
    PACK_STARTTABLE(TBUTTAGBASE),

    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_Image,pack,image,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_SelImage,pack,simage,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_DisImage,pack,dimage,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_Label,pack,label,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_ViewMode,pack,vMode,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_TheBar,pack,tb,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_ID,pack,id,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_LabelPos,pack,lPos,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_Strip,pack,strip,PKCTRL_LONG|PKCTRL_PACKONLY),

    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Borderless,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Borderless),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_NoClick,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_NoClick),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Raised,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Raised),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Scaled,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Scaled),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Sunny,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Sunny),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_EnableKey,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_EnableKey),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Spacer,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_IsSpacer),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Strip,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Strip),

    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_InVirtgroup,pack,flags2,PKCTRL_BIT|PKCTRL_PACKONLY,FLG2_InVirtGroup),

    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Toggle,pack,nflags,PKCTRL_BIT|PKCTRL_PACKONLY,NFLG_Toggle),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Immediate,pack,nflags,PKCTRL_BIT|PKCTRL_PACKONLY,NFLG_Immediate),

    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_HorizTextGfxSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserHorizTextGfxSpacing),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_VertTextGfxSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserVertTextGfxSpacing),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_HorizInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserHorizInnerSpacing),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_TopInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserTopInnerSpacing),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_BottomInnerSpacing,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserBottomInnerSpacing),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Precision,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserPrecision),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_DisMode,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserDisMode),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_Scale,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserScale),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_SpecialSelect,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserSpecialSelect),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_TextOverUseShine,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserTextOverUseShine),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_IgnoreSelImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserIgnoreSelImages),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_IgnoreDisImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserIgnoreDisImages),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_DontMove,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserDontMove),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_NtRaiseActive,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY|PSTF_EXISTS,UFLG_UserNtRaiseActive),

    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_HorizTextGfxSpacing,pack,horizTextGfxSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_VertTextGfxSpacing,pack,vertTextGfxSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_HorizInnerSpacing,pack,horizInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_TopInnerSpacing,pack,topInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_BottomInnerSpacing,pack,bottomInnerSpacing,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_Precision,pack,precision,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_DisMode,pack,disMode,PKCTRL_LONG|PKCTRL_PACKONLY),
    PACK_ENTRY(TBUTTAGBASE,MUIA_TheButton_Scale,pack,scale,PKCTRL_LONG|PKCTRL_PACKONLY),

    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_SpecialSelect,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_SpecialSelect),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_TextOverUseShine,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_TextOverUseShine),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_IgnoreSelImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_IgnoreSelImages),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_IgnoreDisImages,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_IgnoreDisImages),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_DontMove,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_DontMove),
    PACK_LONGBIT(TBUTTAGBASE,MUIA_TheButton_NtRaiseActive,pack,userFlags,PKCTRL_BIT|PKCTRL_PACKONLY,UFLG_NtRaiseActive),

    PACK_NEWOFFSET(MUIA_Disabled),
    PACK_LONGBIT(MUIA_Disabled,MUIA_Disabled,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Disabled),

    PACK_NEWOFFSET(MUIA_Selected),
    PACK_LONGBIT(MUIA_Selected,MUIA_Selected,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Selected),

    PACK_NEWOFFSET(MUIA_ShowMe),
    PACK_LONGBIT(MUIA_ShowMe,MUIA_ShowMe,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_ShowMe),

    PACK_NEWOFFSET(MUIA_Group_Horiz),
    PACK_LONGBIT(MUIA_Group_Horiz,MUIA_Group_Horiz,pack,flags,PKCTRL_BIT|PKCTRL_PACKONLY,FLG_Horiz),

    PACK_ENDTABLE
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    APTR pool;
    struct pack             pack;
    struct TagItem *attrs = msg->ops_AttrList;
    ULONG          nflags, imode;

    if(GetTagData(MUIA_TheButton_MinVer, 0, attrs) > LIB_VERSION)
      return 0;

    if(!(pool = CreatePool(MEMF_ANY, 2048, 1024)))
      return 0;

    memset(&pack,0,sizeof(pack));

    pack.flags |= FLG_ShowMe;
    pack.vMode = MUIV_TheButton_ViewMode_TextGfx;
    pack.lPos  = MUIV_TheButton_LabelPos_Bottom;

    PackStructureTags(&pack,packTable,attrs);

    if (!pack.image || !pack.image->data) pack.vMode = MUIV_TheButton_ViewMode_Text;
    else if (pack.vMode>=MUIV_TheButton_ViewMode_Last) pack.vMode = MUIV_TheButton_ViewMode_Text;
    if (pack.lPos>=MUIV_TheButton_LabelPos_Last) pack.lPos = MUIV_TheButton_LabelPos_Bottom;

    nflags = pack.nflags;

    if (nflags & NFLG_Toggle) imode = MUIV_InputMode_Toggle;
    else if (nflags & NFLG_Immediate) imode = MUIV_InputMode_Immediate;
         else imode = MUIV_InputMode_RelVerify;

    if (pack.flags & FLG_NoClick)
    {
        pack.flags &= ~FLG_EnableKey;
        pack.flags &= ~FLG_Raised;
    }

    if((obj = (Object *)DoSuperNew(cl,obj,
            (pack.flags & FLG_Borderless) ? TAG_IGNORE : MUIA_Frame,   MUIV_Frame_ImageButton,
            (pack.flags & FLG_NoClick) ? TAG_IGNORE : MUIA_CycleChain, TRUE,
            (pack.flags & FLG_NoClick) ? TAG_IGNORE : MUIA_InputMode,  imode,
            MUIA_Font, (pack.vMode==MUIV_TheButton_ViewMode_Text) ? MUIV_Font_Button : MUIV_Font_Tiny,
            (lib_flags & BASEFLG_MUI20) ? TAG_IGNORE : MUIA_CustomBackfill, pack.flags & FLG_Borderless,
            //MUIA_Background,     MUII_ButtonBack,
            TAG_MORE,(ULONG)attrs)))
    {
        struct InstData *data = INST_DATA(cl,obj);
        STRPTR      label;
        ULONG       userFlags;

        data->pool   = pool;
        data->tb     = pack.tb;
        data->id     = pack.id;
        data->strip  = pack.strip;
        data->image  = pack.image;
        data->simage = pack.simage;
        data->dimage = pack.dimage;
        data->vMode  = pack.vMode;
        data->lPos   = pack.lPos;
        data->flags  = pack.flags;
        data->flags2 = pack.flags2;

        if((userFlags = pack.userFlags))
        {
            if(userFlags & UFLG_UserHorizInnerSpacing)
            {
                if (pack.horizInnerSpacing>0 && pack.horizInnerSpacing<=16) data->horizInnerSpacing = pack.horizInnerSpacing;
                else userFlags &= ~UFLG_UserHorizInnerSpacing;
            }

            if (userFlags & UFLG_UserTopInnerSpacing)
            {
                if (pack.topInnerSpacing>0 && pack.topInnerSpacing<=16) data->topInnerSpacing = pack.topInnerSpacing;
                else userFlags &= ~UFLG_UserTopInnerSpacing;
            }

            if (userFlags & UFLG_UserBottomInnerSpacing)
            {
                if (pack.bottomInnerSpacing>0 && pack.bottomInnerSpacing<=16) data->bottomInnerSpacing = pack.bottomInnerSpacing;
                else userFlags &= ~UFLG_UserBottomInnerSpacing;
            }

            if (userFlags & UFLG_UserHorizTextGfxSpacing)
            {
                if (pack.horizTextGfxSpacing>0 && pack.horizTextGfxSpacing<=16) data->horizTextGfxSpacing = pack.horizTextGfxSpacing;
                else userFlags &= ~UFLG_UserHorizTextGfxSpacing;
            }

            if (userFlags & UFLG_UserVertTextGfxSpacing)
            {
                if (pack.vertTextGfxSpacing>0 && pack.vertTextGfxSpacing<=16) data->vertTextGfxSpacing = pack.vertTextGfxSpacing;
                else userFlags &= ~UFLG_UserVertTextGfxSpacing;
            }

            if (userFlags & UFLG_UserPrecision)
            {
                if (pack.precision<MUIV_TheButton_Precision_Last) data->precision = pack.precision;
                else userFlags &= ~UFLG_UserPrecision;
            }

            if (userFlags & UFLG_UserScale)
            {
                if (pack.scale>=25 && pack.scale<=400) data->scale = pack.scale;
                else userFlags &= ~UFLG_UserScale;
            }

            if (userFlags & UFLG_UserDisMode)
            {
                if (pack.disMode<MUIV_TheButton_DisMode_Last)
                {
                    data->disMode = pack.disMode;
                    if (data->disMode==MUIV_TheButton_DisMode_Sunny)
                    	data->flags &= ~FLG_Sunny;
                }
                else userFlags &= ~UFLG_UserDisMode;
            }

            data->userFlags = userFlags;
        }

        if((label = pack.label))
        {
            char *c;

            c = strchr(label,'_');

            if (c && *(c+1))
            {
                data->end = (unsigned char *)(c+1);
                data->cchar = *(c+1);
                if (data->flags & FLG_EnableKey)
                    superset(cl,obj,MUIA_ControlChar,ToLower(data->cchar));
            }

            stripUnderscore((STRPTR)data->blabel,label,STRIP_First);

            data->labelLen = strlen((char *)data->blabel);
        }

        // cleanup the notifyList
        NewList((struct List *)&data->notifyList);
    }
    else
    {
      DeletePool(pool);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;
  APTR  pool = data->pool;
  ULONG res;

  ENTER();

  // cleanup the notifyList
  for(node = data->notifyList.mlh_Head; node->mln_Succ; )
  {
    struct ButtonNotify *notify = (struct ButtonNotify *)node;

    // before we remove the node we have to save the pointer to the next one
    node = node->mln_Succ;

    // Remove node from list
    Remove((struct Node *)notify);

    // Free everything of the node
    freeVecPooled(pool, notify);
  }

  // call the super method
  res = DoSuperMethodA(cl, obj, msg);

  // delete our memory pool
  DeletePool(pool);

  RETURN(res);
  return res;
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
  struct InstData *data = INST_DATA(cl,obj);


  switch(msg->opg_AttrID)
  {
    case MUIA_ShowMe:                  *msg->opg_Storage = (data->flags & FLG_ShowMe) ? TRUE : FALSE; return TRUE;

    case MUIA_TheButton_Spacer:        *msg->opg_Storage = (data->flags & FLG_IsSpacer) ? MUIV_TheButton_Spacer_Image : MUIV_TheButton_Spacer_None; return TRUE;
    case MUIA_TheButton_TheBar:        *msg->opg_Storage = (ULONG)data->tb; return TRUE;
    case MUIA_TheButton_ViewMode:      *msg->opg_Storage = data->vMode; return TRUE;
    case MUIA_TheButton_Raised:        *msg->opg_Storage = (data->flags & FLG_Raised) ? TRUE : FALSE; return TRUE;
    case MUIA_TheButton_Scaled:        *msg->opg_Storage = (data->flags & FLG_Scaled) ? TRUE : FALSE; return TRUE;
    case MUIA_TheButton_LabelPos:      *msg->opg_Storage = data->lPos; return TRUE;
    case MUIA_TheButton_EnableKey:     *msg->opg_Storage = (data->flags & FLG_EnableKey) ? TRUE : FALSE; return TRUE;
    case MUIA_TheButton_DisMode:       *msg->opg_Storage = data->disMode; return TRUE;
    case MUIA_TheButton_NtRaiseActive: *msg->opg_Storage = (data->userFlags & UFLG_NtRaiseActive) ? TRUE : FALSE; return TRUE;
    case MUIA_TheButton_NotifyList:    *msg->opg_Storage = (ULONG)&data->notifyList; return TRUE;

    case MUIA_Version:                 *msg->opg_Storage = LIB_VERSION; return TRUE;
    case MUIA_Revision:                *msg->opg_Storage = LIB_REVISION; return TRUE;

    default:
      return DoSuperMethodA(cl,obj,(Msg)msg);
  }
}

/***********************************************************************/

static void
addRemEventHandler(struct IClass *cl, Object *obj, struct InstData *data)
{
  ENTER();

  // we only do modifications if the button is
  // visible at all
  if(data->flags & FLG_Visible)
  {
    ULONG catchableEvents = 0;

    if(!(data->flags & FLG_Disabled) &&
       (((data->flags & (FLG_Raised|FLG_Sunny)) && !(data->flags & FLG_Selected)) || (data->flags2 & FLG2_Special)))
    {
      // now we also catch mousemove events
      catchableEvents |= ((lib_flags & BASEFLG_MUI20) ? IDCMP_MOUSEOBJECT : IDCMP_MOUSEMOVE);
    }

    // install the event handler
    if(!(data->flags & FLG_Handler) || catchableEvents != data->eh.ehn_Events)
    {
      // in case a handler is already installed we have to remove it
      // previously
      if(data->flags & FLG_Handler)
        DoMethod(_win(obj), MUIM_Window_RemEventHandler, (ULONG)&data->eh);

      // now add the new handler
      data->eh.ehn_Priority = 0;
      data->eh.ehn_Flags    = MUI_EHF_GUIMODE;
      data->eh.ehn_Object   = obj;
      data->eh.ehn_Class    = cl;
      data->eh.ehn_Events   = catchableEvents | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS; // always catch RAWKEY&MOUSEBUTTONS
      DoMethod(_win(obj), MUIM_Window_AddEventHandler, (ULONG)&data->eh);

      // flag the event handler as being installed
      data->flags |= FLG_Handler;
    }
  }

  LEAVE();
}

/***********************************************************************/

#define _between(a,x,b)      ((x)>=(a) && (x)<=(b))
#define _isinobject(obj,x,y) (_between(_left(obj),(x),_right(obj)) && _between(_top(obj),(y),_bottom(obj)))

static ULONG
checkIn(Object *obj,struct InstData *data,WORD x,WORD y)
{
  ULONG in;

  ENTER();

  if((in = _isinobject(obj,x,y)) && (data->flags & FLG_IsInVirtgroup))
  {
    Object *p = obj;

    for (;;)
    {
      if((p = (Object *)xget(p, MUIA_Parent)) == NULL)
        break;

      if(!_isinobject(p,x,y))
      {
        in = FALSE;
        break;
      }
    }
  }

  RETURN(in);
  return in;
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct InstData    *data = INST_DATA(cl,obj);
    struct TagItem *tag, *vmt, *rat, *sct, *sut, *lpt, *ekt, *ract;
    struct TagItem          *tstate;
    ULONG          redraw, setidcmp, back, sel, res, pressed, over;

    redraw = setidcmp = back = sel = pressed = over = FALSE;
    vmt = rat = sct = sut = lpt = ekt = ract = NULL;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem(&tstate)); )
    {
        ULONG tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_TheButton_MouseOver:
                if (BOOLSAME(tidata,data->flags & FLG_MouseOver)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata)
                    {
                        data->flags |= FLG_MouseOver;
                        over = TRUE;
                    }
                    else data->flags &= ~FLG_MouseOver;
                    back = TRUE;
                }
                break;

            case MUIA_Group_Horiz:
                if (tidata) data->flags |= FLG_Horiz;
                else data->flags &= ~FLG_Horiz;
                break;

            case MUIA_TheButton_Sunny:
                if (BOOLSAME(tidata,data->flags & FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny))
                {
                    sut = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata) data->flags |= FLG_Sunny;
                    else data->flags &= ~FLG_Sunny;
                }
                break;

            case MUIA_Selected:
                if ((data->flags & FLG_IsSpacer) || BOOLSAME(tidata,data->flags & FLG_Selected))
                {
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata)
                    {
                        data->flags |= FLG_Selected;
                    }
                    else
                    {
                        data->flags &= ~FLG_Selected;
                    }

                    sel = TRUE;
                    setidcmp = TRUE;
                }
                break;

            case MUIA_Disabled:
                if ((data->flags & FLG_IsSpacer) || BOOLSAME(tidata,data->flags & FLG_Disabled))
                {
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata) data->flags |= FLG_Disabled;
                    else data->flags &= ~FLG_Disabled;
                    setidcmp = back = TRUE;
                }
                break;

            case MUIA_ShowMe:
                if (BOOLSAME(tidata,data->flags & FLG_ShowMe)) tag->ti_Tag = TAG_IGNORE;
                else
                {
                    if (tidata) data->flags |= FLG_ShowMe;
                    else data->flags &= ~FLG_ShowMe;
                }
                break;

            case MUIA_TheButton_TheBar:
                data->tb = (Object *)tidata;
                break;

            case MUIA_TheButton_Quiet:
                if (tidata) data->flags |= FLG_NoNotify;
                else data->flags &= ~FLG_NoNotify;
                break;

            case MUIA_TheButton_Raised:
                if ((data->flags & (FLG_NoClick|FLG_IsSpacer)) || !(data->flags & FLG_Borderless) || BOOLSAME(tidata,data->flags & FLG_Raised))
                {
                    rat = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata) data->flags |= FLG_Raised;
                    else data->flags &= ~FLG_Raised;
                    setidcmp = TRUE;
                }
                break;

            case MUIA_TheButton_ViewMode:
                if ((data->flags & FLG_IsSpacer) || !data->image || !data->image->data || (tidata>=MUIV_TheButton_ViewMode_Last) || (tidata==data->vMode))
                {
                    vmt = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else data->vMode = tidata;
                break;

            case MUIA_TheButton_Scaled:
                if (BOOLSAME(tidata,data->flags & FLG_Scaled))
                {
                    sct = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                    if (tidata) data->flags |= FLG_Scaled;
                    else data->flags &= ~FLG_Scaled;
                break;

            case MUIA_TheButton_LabelPos:
                if ((data->flags & FLG_IsSpacer) || (tidata>=MUIV_TheButton_LabelPos_Last) || (tidata==data->lPos))
                {
                    lpt = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else data->lPos = tidata;
                break;

            case MUIA_TheButton_EnableKey:
                if ((data->flags & FLG_IsSpacer) || BOOLSAME(tidata,data->flags & FLG_EnableKey))
                {
                    ekt = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata)
                    {
                        data->flags |= FLG_EnableKey;
                        if (data->cchar) superset(cl,obj,MUIA_ControlChar,ToLower(data->cchar));
                    }
                    else
                    {
                        data->flags &= ~FLG_EnableKey;
                        superset(cl,obj,MUIA_ControlChar,0);
                    }
                }
                break;

            case MUIA_TheBar_Limbo:
                if (tidata)
                {
                    data->flags2 |= FLG2_Limbo|FLG2_Special;
                    addRemEventHandler(cl, obj, data);
                }
                else data->flags2 &= ~FLG2_Limbo;
                break;

            case MUIA_TheButton_NtRaiseActive:
                if (BOOLSAME(tidata,data->userFlags & UFLG_NtRaiseActive))
                {
                    ract = tag;
                    tag->ti_Tag = TAG_IGNORE;
                }
                else
                {
                    if (tidata) data->userFlags |= UFLG_NtRaiseActive;
                    else data->userFlags &= ~UFLG_NtRaiseActive;
                    setidcmp = TRUE;
                }
                break;

            case MUIA_Pressed:
                pressed = TRUE;
                break;
        }
    }

    if (setidcmp)
    {
        addRemEventHandler(cl,obj,data);

        if (data->flags & FLG_Disabled) data->flags &= ~FLG_MouseOver;
        else
            if (data->flags & FLG_Visible)
            {
                if (checkIn(obj,data,_window(obj)->MouseX,_window(obj)->MouseY)) data->flags |= FLG_MouseOver;
                else data->flags &= ~FLG_MouseOver;
                back = TRUE;
            }
    }

    if (data->flags & FLG_Borderless)
    {
        //if ((data->flags & FLG_Selected) && !sel) back = FALSE;
        if (sel) back = TRUE;

        if (back)
        {
            if (!pressed || sel)
            {
                data->flags |= FLG_RedrawBack;
                redraw = TRUE;
            }

            if (!(data->flags & FLG_Disabled))
            {
                if ((data->flags & FLG_Raised) && (data->flags & FLG_MouseOver))
                {
                    if (data->flags & FLG_Selected) superset(cl,obj,MUIA_Background,data->parentBack);
                    else superset(cl,obj,MUIA_Background,data->activeBack);
                }
                else
                {
                    superset(cl,obj,MUIA_Background,data->parentBack);
                }
            }
            else
            {
                superset(cl,obj,MUIA_Background,data->parentBack);
            }
        }
    }
    else
    {
        if (data->flags & FLG_Sunny) redraw = back;
    }

    res = DoSuperMethodA(cl,obj,(Msg)msg);
    data->flags &= ~FLG_RedrawBack;
    if (redraw) MUI_Redraw(obj,MADF_DRAWOBJECT);

    if (vmt) vmt->ti_Tag = MUIA_TheButton_ViewMode;
    if (rat) rat->ti_Tag = MUIA_TheButton_Raised;
    if (sct) sct->ti_Tag = MUIA_TheButton_Scaled;
    if (sut) sut->ti_Tag = MUIA_TheButton_Sunny;
    if (lpt) lpt->ti_Tag = MUIA_TheButton_LabelPos;
    if (ekt) ekt->ti_Tag = MUIA_TheButton_EnableKey;
    if (ract) ract->ti_Tag = MUIA_TheButton_NtRaiseActive;

    if (over && data->tb) set(data->tb,MUIA_TheBar_MouseOver,data->id);
    return res;
}

/***********************************************************************/

static ULONG
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData    *data = INST_DATA(cl,obj);
    Object                  *parent;
    APTR                    pen;
    ULONG                   *val;

    /* Background (BEFORE SUPER METHOD!) */
    superget(cl,obj,MUIA_Parent,&parent);
    data->parentBack = parent ? _backspec(parent) : (APTR)MUII_WindowBack;
    superset(cl,obj,MUIA_Background,(data->flags & FLG_Borderless) ? (ULONG)data->parentBack : MUII_ButtonBack);

    /* Super method */
    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (!(data->flags2 & FLG2_Limbo))
    {
        /* Active background */
        if (lib_flags & BASEFLG_MUI4)
        {
            getconfigitem(cl,obj,MUICFG_TheBar_ButtonBack,&pen);
            data->activeBack = pen;
        }
        else
        {
            if ((getconfigitem(cl,obj,MUICFG_TheBar_UseButtonBack,&val) ? *val : MUIDEF_TheBar_UseButtonBack) &&
                getconfigitem(cl,obj,MUICFG_TheBar_ButtonBack,&pen))
                data->activeBack = pen;
            else data->activeBack = NULL;
        }

        /* Frame shine */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_FrameShinePen,&pen)) pen = MUIDEF_TheBar_FrameShinePen;
        data->shine = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        /* Frame shadow */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_FrameShadowPen,&pen)) pen = MUIDEF_TheBar_FrameShadowPen;
        data->shadow = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        /* Frame style */
        data->fStyle = getconfigitem(cl,obj,MUICFG_TheBar_FrameStyle,&val) ?
            *val : MUIDEF_TheBar_FrameStyle;

        /* Disabled body */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_DisBodyPen,&pen)) pen = MUIDEF_TheBar_DisBodyPen;
        data->disBodyPen = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        /* Disabled shadow */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_DisShadowPen,(ULONG)&pen)) pen = MUIDEF_TheBar_DisShadowPen;
        data->disShadowPen = MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)pen,0);

        /* Text font */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_TextFont,&pen) ||
            !(data->tf = openFont((STRPTR)pen))) data->tf = _font(obj);
        else data->flags |= FLG_CloseTF;

        /* TextGfx font */
        if (!getconfigitem(cl,obj,MUICFG_TheBar_TextGfxFont,&pen) ||
            !(data->tgf = openFont((STRPTR)pen))) data->tgf = _font(obj);
        else data->flags |= FLG_CloseTGF;

        /* TextGfx horiz space */
        if (!(data->userFlags & UFLG_UserHorizTextGfxSpacing))
        {
            data->horizTextGfxSpacing = getconfigitem(cl,obj,MUICFG_TheBar_HorizTextGfxSpacing,&val) ?
                *val : MUIDEF_TheBar_HorizTextGfxSpacing;
        }

        /* TextGfx vert space */
        if (!(data->userFlags & UFLG_UserVertTextGfxSpacing))
        {
            data->vertTextGfxSpacing = getconfigitem(cl,obj,MUICFG_TheBar_VertTextGfxSpacing,&val) ?
                *val : MUIDEF_TheBar_VertTextGfxSpacing;
        }

        /* Inner spaces */
        if (!(data->userFlags & UFLG_UserHorizInnerSpacing))
        {
            data->horizInnerSpacing = getconfigitem(cl,obj,MUICFG_TheBar_HorizInnerSpacing,&val) ?
                *val : MUIDEF_TheBar_HorizInnerSpacing;
        }

        if (!(data->userFlags & UFLG_UserTopInnerSpacing))
        {
            data->topInnerSpacing = getconfigitem(cl,obj,MUICFG_TheBar_TopInnerSpacing,&val) ?
                *val : MUIDEF_TheBar_TopInnerSpacing;
        }

        if (!(data->userFlags & UFLG_UserBottomInnerSpacing))
        {
            data->bottomInnerSpacing = getconfigitem(cl,obj,MUICFG_TheBar_BottomInnerSpacing,&val) ?
                *val : MUIDEF_TheBar_BottomInnerSpacing;
        }

        /* Precision */
        if (!(data->userFlags & UFLG_UserPrecision))
        {
            switch (getconfigitem(cl,obj,MUICFG_TheBar_Precision,&val) ? *val : MUIDEF_TheBar_Precision)
            {
                case MUIV_TheButton_Precision_Icon:  data->precision = PRECISION_ICON; break;
                case MUIV_TheButton_Precision_Image: data->precision = PRECISION_IMAGE; break;
                case MUIV_TheButton_Precision_Exact: data->precision = PRECISION_EXACT; break;
                default: data->precision = PRECISION_GUI; break; //MUIV_TheButton_Precision_GUI
            }
        }

        /* DisMode */
        if (!(data->userFlags & UFLG_UserDisMode))
        {
            data->disMode = getconfigitem(cl,obj,MUICFG_TheBar_DisMode,&val) ?
                *val : MUIDEF_TheBar_DisMode;

            if (data->disMode==MUIV_TheButton_DisMode_Sunny)
            	data->flags &= ~FLG_Sunny;
        }

        /* Scale ratio */
        if (!(data->userFlags & UFLG_UserScale))
        {
            data->scale = getconfigitem(cl,obj,MUICFG_TheBar_Scale,&val) ?
                *val : MUIDEF_TheBar_Scale;
        }

        /* SpecialSelect */
        if (!(data->userFlags & UFLG_UserSpecialSelect))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_SpecialSelect,&val) ? *val : MUIDEF_TheBar_SpecialSelect)
                data->userFlags |= UFLG_SpecialSelect;
            else data->userFlags &= ~UFLG_SpecialSelect;
        }

        /* TextOverUseShine */
        if (!(data->userFlags & UFLG_UserTextOverUseShine))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_TextOverUseShine,&val) ? *val : MUIDEF_TheBar_TextOverUseShine)
                data->userFlags |= UFLG_TextOverUseShine;
            else data->userFlags &= ~UFLG_TextOverUseShine;
        }

        /* IgnoreSelImages */
        if (!(data->userFlags & UFLG_UserIgnoreSelImages))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreSelImages,&val) ? *val : MUIDEF_TheBar_IgnoreSelImages)
                data->userFlags |= UFLG_IgnoreSelImages;
            else data->userFlags &= ~UFLG_IgnoreSelImages;
        }

        /* IgnoreDisImages */
        if (!(data->userFlags & UFLG_UserIgnoreDisImages))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_IgnoreDisImages,&val) ? *val : MUIDEF_TheBar_IgnoreDisImages)
                data->userFlags |= UFLG_IgnoreDisImages;
            else data->userFlags &= ~UFLG_IgnoreDisImages;
        }

        /* DontMove */
        if (!(data->userFlags & UFLG_UserDontMove))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_DontMove,&val) ? *val : MUIDEF_TheBar_DontMove)
                data->userFlags |= UFLG_DontMove;
            else data->userFlags &= ~UFLG_DontMove;
        }

        /* NtRaiseActive */
        if (!(data->userFlags & UFLG_UserNtRaiseActive))
        {
            if (getconfigitem(cl,obj,MUICFG_TheBar_NtRaiseActive,&val) ? *val : MUIDEF_TheBar_NtRaiseActive)
                data->userFlags |= UFLG_NtRaiseActive;
            else data->userFlags &= ~UFLG_NtRaiseActive;
        }

        /* Compute frame size */
        data->fSize = (data->flags & FLG_Borderless) ? ((_riflags(obj) & MUIMRI_THINFRAMES) ? 1 : 2) : 0;

        /* Derive GFX env info */
        data->screen = _screen(obj);
        data->screenDepth = GetBitMapAttr(data->screen->RastPort.BitMap,BMA_DEPTH);
        if (CyberGfxBase && IsCyberModeID(GetVPModeID(&data->screen->ViewPort)))
        {
            data->flags |= FLG_CyberMap;
            if (data->screenDepth>8) data->flags |= FLG_CyberDeep;
        }

        if (!(data->flags & FLG_Strip) && (data->vMode!=MUIV_TheButton_ViewMode_Text))
            build(data);
    }
    else data->flags2 &= ~FLG2_Limbo;

    if (!(data->flags2 & FLG2_InVirtGroup))
    {
        /* Virtgroup hack */
        parent = obj;
        for (;;)
        {
            if((parent = (Object *)xget(parent, MUIA_Parent)) == NULL)
                break;

            if((pen = (APTR)xget(parent, MUIA_Virtgroup_Top)))
            {
                data->flags |= FLG_IsInVirtgroup;
                break;
            }
        }
    }
    else data->flags |= FLG_IsInVirtgroup;

    /* Done */
    data->flags |= FLG_Setup;

    return TRUE;
}

/***********************************************************************/

static ULONG
mBuild(struct IClass *cl,Object *obj,UNUSED Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);

  build(data);

  return 0;
}

/***********************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    if (!(data->flags2 & FLG2_Limbo))
    {
        MUI_ReleasePen(muiRenderInfo(obj),data->shine);
        MUI_ReleasePen(muiRenderInfo(obj),data->shadow);
        MUI_ReleasePen(muiRenderInfo(obj),data->disBodyPen);
        MUI_ReleasePen(muiRenderInfo(obj),data->disShadowPen);

        if (data->flags & FLG_CloseTF) CloseFont(data->tf);
        if (data->flags & FLG_CloseTGF) CloseFont(data->tgf);

        freeBitMaps(data);

        data->flags &= ~(FLG_CyberMap|FLG_CyberDeep|FLG_CloseTF|FLG_CloseTGF);
    }

    data->flags &= ~(FLG_Setup|FLG_IsInVirtgroup);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result = FALSE;

  ENTER();

  if(DoSuperMethodA(cl,obj,msg))
  {
    data->rp = *_rp(obj);
    data->flags |= FLG_Visible;

    addRemEventHandler(cl,obj,data);

    // peek the current qualifier state
    data->qualifier = peekQualifier();

    result = TRUE;
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mHide(struct IClass *cl,Object *obj,Msg msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result;

  ENTER();

  // remove the event handler if installed
  if(data->flags & FLG_Handler)
    DoMethod(_win(obj),MUIM_Window_RemEventHandler,(ULONG)&data->eh);

  data->flags &= ~(FLG_Visible|FLG_Handler);

  result = DoSuperMethodA(cl,obj,msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct InstData *data = INST_DATA(cl,obj);
    UWORD       mw, mh;

    DoSuperMethodA(cl,obj,(Msg)msg);

    mw = msg->MinMaxInfo->MinWidth;
    mh = msg->MinMaxInfo->MinHeight;

    if (data->labelLen)
    {
        struct RastPort rp;

        memcpy(&rp,&_screen(obj)->RastPort,sizeof(rp));

        if (data->vMode==MUIV_TheButton_ViewMode_Text) SetFont(&rp,data->tf);
        else SetFont(&rp,data->tgf);

        TextExtent(&rp,(STRPTR)data->blabel,data->labelLen,&data->lInfo);

        if (data->end)
        {
            TextExtent(&rp,(STRPTR)data->end,strlen((char *)data->end),&data->eInfo);
            TextExtent(&rp,(STRPTR)&data->cchar,1,&data->ccInfo);
        }
    }

    mw += 2*(data->fSize+data->horizInnerSpacing);
    mh += (data->fSize ? 2 : 0)+data->topInnerSpacing+data->bottomInnerSpacing;

    switch(data->vMode)
    {
        case MUIV_TheButton_ViewMode_Text:
            mw += data->lInfo.te_Width;
            mh += data->lInfo.te_Height;
            break;

        case MUIV_TheButton_ViewMode_Gfx:
            mw += data->imgWidth;
            mh += data->imgHeight;
            break;

        case MUIV_TheButton_ViewMode_TextGfx:
            if (data->lPos==MUIV_TheButton_LabelPos_Left || data->lPos==MUIV_TheButton_LabelPos_Right)
            {
                mw += data->lInfo.te_Width+data->imgWidth+((data->lInfo.te_Width && data->imgWidth) ? data->horizTextGfxSpacing : 0);
                mh += (data->lInfo.te_Height>data->imgHeight) ? data->lInfo.te_Height : data->imgHeight;

                if (data->userFlags & UFLG_SpecialSelect)
                {
                    mw++;
                    mh++;
                }
            }
            else
            {
                mw += (data->lInfo.te_Width > (LONG)data->imgWidth) ? (ULONG)data->lInfo.te_Width : data->imgWidth;
                mh += data->lInfo.te_Height+data->imgHeight+((data->lInfo.te_Height && data->imgHeight) ? data->vertTextGfxSpacing : 0);

                if (data->userFlags & UFLG_SpecialSelect)
                {
                    mw++;
                    mh++;
                }
            }
            break;
    }

    msg->MinMaxInfo->MinWidth  = msg->MinMaxInfo->DefWidth  = msg->MinMaxInfo->MaxWidth  = mw;
    msg->MinMaxInfo->MinHeight = msg->MinMaxInfo->DefHeight = msg->MinMaxInfo->MaxHeight = mh;

    msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX;
    msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;

    /*NewRawDoFmt("AskMinMax: %lx - %ld %ld %ld %ld %ld %ld\n",1,1,obj,
        msg->MinMaxInfo->MinWidth,msg->MinMaxInfo->DefWidth,msg->MinMaxInfo->MaxWidth,
        msg->MinMaxInfo->MinHeight,msg->MinMaxInfo->DefHeight,msg->MinMaxInfo->MaxHeight);*/

    /*kprintf("AskMinMax: %lx - %ld %ld %ld %ld %ld %ld\n",obj,
        msg->MinMaxInfo->MinWidth,msg->MinMaxInfo->DefWidth,msg->MinMaxInfo->MaxWidth,
        msg->MinMaxInfo->MinHeight,msg->MinMaxInfo->DefHeight,msg->MinMaxInfo->MaxHeight);*/

    return 0;
}

/***********************************************************************/

static void
drawText(struct InstData *data,struct RastPort *rp)
{
    if (data->labelLen)
    {
        Text(rp,(STRPTR)data->blabel,data->labelLen);

        if (data->end && (data->flags & FLG_EnableKey))
        {
            WORD x, y, ux, uy;

            x = rp->cp_x;
            y = rp->cp_y;

            ux = x-data->eInfo.te_Width;
            uy = y+1;

            Move(rp,ux,uy);
            Draw(rp,ux+data->ccInfo.te_Extent.MaxX,uy);
            Move(rp,x,y);
        }
    }
}

/***********************************************************************/

static ULONG
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct InstData    *data = INST_DATA(cl,obj);
    ULONG          flags = data->flags;

    //if (flags & FLG_RedrawBack) return 0;

    DoSuperMethodA(cl,obj,(Msg)msg);

    //Printf("Draw %lx %lx %lx\n",obj,flags & FLG_RedrawBack,msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT));

    if (!(flags & FLG_RedrawBack) && (msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT)))
    {
        struct RastPort *rp = &data->rp;
        ULONG           bl = flags & FLG_Borderless,
                                 se = flags & FLG_Selected,
                                 ov = flags & FLG_MouseOver,
                                 ra = flags & FLG_Raised,
                                 di = flags & FLG_Disabled,
                                 done = FALSE,
                                 strip = (flags & FLG_Strip) && !(flags & FLG_Scaled);
        UWORD           txp = 0, typ = 0, ixp = 0, iyp = 0,
                                 iw = data->imgWidth, ih = data->imgHeight,
                                 tw = data->lInfo.te_Width, th  = data->lInfo.te_Height,
                                 tmy = data->lInfo.te_Extent.MinY, vm = data->vMode,
                                 hisp = data->horizInnerSpacing, tisp = data->topInnerSpacing;
        WORD            mt = _mtop(obj),  ml = _mleft(obj),
                                 mw = _mwidth(obj), mh = _mheight(obj);

        switch(data->lPos)
        {
            case MUIV_TheButton_LabelPos_Top:
                txp = ml+(mw-tw)/2;
                typ = mt+(data->fSize ? 1 : 0)+tisp-tmy;

                ixp = ml+(mw-iw)/2;
                iyp = mt+(data->fSize ? 1 : 0)+tisp;
                if (data->labelLen && vm==MUIV_TheButton_ViewMode_TextGfx) iyp += th+data->vertTextGfxSpacing;
                break;

            case MUIV_TheButton_LabelPos_Bottom:
                txp = ml+(mw-tw)/2;
                typ = mt+(data->fSize ? 1 : 0)+tisp-tmy;
                if (ih && vm==MUIV_TheButton_ViewMode_TextGfx) typ += ih+data->vertTextGfxSpacing;

                ixp = ml+(mw-iw)/2;
                iyp = mt+(data->fSize ? 1 : 0)+tisp;
                break;

            case MUIV_TheButton_LabelPos_Right:
            {
                UWORD s = 0;

                switch(vm)
                {
                    case MUIV_TheButton_ViewMode_Text:
                        s = tw;
                        break;

                    case MUIV_TheButton_ViewMode_Gfx:
                        s = iw;
                        break;

                    case MUIV_TheButton_ViewMode_TextGfx:
                        s = tw+iw+((tw && iw) ? data->horizTextGfxSpacing : 0);
                        break;
                }

                if (vm!=MUIV_TheButton_ViewMode_TextGfx || (data->flags & FLG_Horiz))
                {
                    ixp = ml+(mw-s)/2;
                    iyp = mt+(mh-ih)/2;

                    txp = ixp;
                    if (iw && vm==MUIV_TheButton_ViewMode_TextGfx) txp += iw+data->horizTextGfxSpacing;
                    typ = mt+(mh-th)/2-tmy;
                }
                else
                {
                    ixp = ml+hisp;
                    iyp = mt+(mh-ih)/2;

                    txp = ml+hisp+iw+((iw || tw) ? data->horizTextGfxSpacing : 0);
                    typ = mt+(mh-th)/2-tmy;
                }
                break;
            }

            case MUIV_TheButton_LabelPos_Left:
            {
                UWORD s = 0;

                switch(vm)
                {
                    case MUIV_TheButton_ViewMode_Text:
                        s = tw;
                        break;

                    case MUIV_TheButton_ViewMode_Gfx:
                        s = iw;
                        break;

                    case MUIV_TheButton_ViewMode_TextGfx:
                        s = tw+iw+((tw && iw) ? data->horizTextGfxSpacing : 0);
                        break;
                }

                if (vm!=MUIV_TheButton_ViewMode_TextGfx || (data->flags & FLG_Horiz))
                {
                    txp = ml+(mw-s)/2;
                    typ = mt+(mh-th)/2-tmy;

                    ixp = txp;
                    if (data->labelLen && vm==MUIV_TheButton_ViewMode_TextGfx) ixp += tw+data->horizTextGfxSpacing;
                    iyp = mt+(mh-ih)/2;
                }
                else
                {
                    ixp = _mright(obj)-hisp-iw;
                    iyp = mt+(mh-ih)/2;

                    txp = ixp-((iw || tw) ? data->horizTextGfxSpacing : 0)-tw;
                    typ = mt+(mh-th)/2-tmy;
                }
                break;
            }
        }

        if (!(data->userFlags & UFLG_DontMove) && (se || ((data->userFlags & UFLG_SpecialSelect) && ov)))
        {
            txp++;
            typ++;
            ixp++;
            iyp++;
        }

        SetFont(rp,(vm==MUIV_TheButton_ViewMode_Text) ? data->tf : data->tgf);

        if (di)
        {
            if (vm!=MUIV_TheButton_ViewMode_Text)
            {
                struct BitMap *bm;
                APTR          mask;
                UWORD         x, y;
                #if defined(WITH_ALPHA)
                UBYTE	        *chunky = NULL;
                ULONG	        useChunky = data->image->flags & BRFLG_AlphaMask;
                #endif

                if (data->flags & FLG_Sunny)
                {
                    if (strip)
                    {
                        #if defined(WITH_ALPHA)
                        if (useChunky) chunky = data->strip->dgchunky;
                        #endif

                        if((bm = data->strip->dgreyBM))
                          mask = data->strip->dmask;
                        else
                          mask = data->strip->mask;

                        x = data->image->left;
                        y = data->image->top;
                    }
                    else
                    {
                        #if defined(WITH_ALPHA)
                        if (useChunky) chunky = data->dgchunky;
                        #endif

                        if((bm = data->dgreyBM))
                          mask = data->dmask;
                        else
                          mask = data->mask;

                        x = y = 0;
                    }
                }
                else
                {
                    if (data->disMode==MUIV_TheButton_DisMode_Sunny)
                    {
                        if (strip)
                        {
                            #if defined(WITH_ALPHA)
    	                    if (useChunky) chunky = data->strip->gchunky;
                            #endif

                            bm = data->strip->greyBM;
                            mask = data->strip->mask;

                            x = data->image->left;
                            y = data->image->top;
                        }
                        else
                        {
                            #if defined(WITH_ALPHA)
    	                    if (useChunky) chunky = data->gchunky;
                            #endif

                            bm = data->greyBM;
                            mask = data->mask;

                            x = y = 0;
                        }
                    }
                    else
                        if (strip)
                        {
                            #if defined(WITH_ALPHA)
    	                    if (useChunky) chunky = data->strip->dnchunky;
                            #endif

                            if((bm = data->strip->dnormalBM))
                              mask = data->strip->dmask;
                            else
                              mask = data->strip->mask;

                            x = data->image->left;
                            y = data->image->top;
                        }
                        else
                        {
                            #if defined(WITH_ALPHA)
    	                    if (useChunky) chunky = data->dnchunky;
                            #endif

                            if((bm = data->dnormalBM))
                              mask = data->dmask;
                            else
                              mask = data->mask;

                            x = y = 0;
                        }
                }

                #if defined(WITH_ALPHA)
                if((done = (bm || chunky)))
                #else
                if((done = (ULONG)bm))
                #endif
                {
                    //NewRawDoFmt(">>> 1 %lx %lx %lx %ld\n",1,1,bm,chunky,mask,data->disMode);
                    #if defined(WITH_ALPHA)
                    if (chunky)
                    {
	                    if(data->image->flags & BRFLG_EmpytAlpha)
                        WritePixelArray(chunky,x,y,(data->flags & FLG_Scaled) ? iw*4 : data->image->dataWidth*4,rp,ixp,iyp,iw,ih,RECTFMT_ARGB);
                      else
                        WritePixelArrayAlpha(chunky,x,y,(data->flags & FLG_Scaled) ? iw*4 : data->image->dataWidth*4,rp,ixp,iyp,iw,ih,lib_alpha);
                    }
                    else
                    #endif
                    {
                        if(bm)
                        {
                    	    if (mask) BltMaskBitMapRastPort(bm,x,y,rp,ixp,iyp,iw,ih,(ABC|ABNC|ANBC),mask);
                    	    else BltBitMapRastPort(bm,x,y,rp,ixp,iyp,iw,ih,0xc0);
                        }
                    }
                }
                else
                {
                    if (mask && (data->disMode==MUIV_TheButton_DisMode_Shape))
                    {
                        if (strip)
                        {
                            struct BitMap *tbm;

                            if((tbm = AllocBitMap(iw,ih,1,0,NULL)))
                            {
                                struct BitMap sbm;

                                memset(&sbm,0,sizeof(sbm));
                                sbm.BytesPerRow = RAWIDTH(data->image->dataWidth);
                                sbm.Rows        = data->image->dataHeight;
                                sbm.Depth       = 1;
                                sbm.Planes[0]   = mask;

                                BltBitMap(&sbm,x,y,tbm,0,0,iw,ih,0xc0,0xff,NULL);

                                SetAPen(rp,MUIPEN(data->disShadowPen));
                                BltTemplate(tbm->Planes[0],0,RAWIDTH(iw),rp,ixp+1,iyp+1,iw,ih);

                                SetAPen(rp,MUIPEN(data->disBodyPen));
                                BltTemplate(tbm->Planes[0],0,RAWIDTH(iw),rp,ixp,iyp,iw,ih);

                                WaitBlit();
                                FreeBitMap(tbm);

                                done = TRUE;
                            }
                        }
                        else
                        {
                            SetAPen(rp,MUIPEN(data->disShadowPen));
                            BltTemplate(mask,0,(iw+15)>>3 & 0xFFFE,rp,ixp+1,iyp+1,iw,ih);

                            SetAPen(rp,MUIPEN(data->disBodyPen));
                            BltTemplate(mask,0,(iw+15)>>3 & 0xFFFE,rp,ixp,iyp,iw,ih);

                            done = TRUE;
                        }
                    }
                }

                if (done && (vm!=MUIV_TheButton_ViewMode_Gfx))
                {
                    SetAPen(rp,MUIPEN(data->disShadowPen));
                    Move(rp,txp+1,typ+1);
                    drawText(data,rp);

                    SetAPen(rp,MUIPEN(data->disBodyPen));
                    Move(rp,txp,typ);
                    drawText(data,rp);
                }
            }
        }

        if (!done)
        {
            if (vm!=MUIV_TheButton_ViewMode_Gfx)
            {
                SetAPen(rp,di ? MUIPEN(data->disShadowPen) : _pens(obj)[((data->userFlags & UFLG_TextOverUseShine) && ov && !se) ? MPEN_SHINE : MPEN_TEXT]);
                Move(rp,txp,typ);
                drawText(data,rp);
            }

            if (vm!=MUIV_TheButton_ViewMode_Text)
            {
                struct BitMap *bm;
                APTR          mask;
                UWORD         x = 0, y = 0;
                #if defined(WITH_ALPHA)
                UBYTE	        *chunky = NULL;
                ULONG	        useChunky = data->image->flags & BRFLG_AlphaMask;
                #endif

                if (strip)
                {
	                if (data->flags & FLG_Scaled)
                    {
                        #if defined(WITH_ALPHA)
                        if (useChunky) chunky = data->nchunky;
                        #endif

                        bm = data->normalBM;
                        mask = data->mask;
                    }
                    else
                    {
                        #if defined(WITH_ALPHA)
                        if (useChunky) chunky = data->strip->nchunky;
                        #endif

                        bm = data->strip->normalBM;
                        mask = data->strip->mask;

                        x = data->image->left;
                        y = data->image->top;
                    }
                }
                else
                {
                    #if defined(WITH_ALPHA)
                    if (useChunky) chunky = data->nchunky;
                    #endif

                    bm = data->normalBM;
                    mask = data->mask;
                }

                if (se)
                {
                    if (strip)
                    {
    	                if (data->flags & FLG_Scaled)
                        {
                            #if defined(WITH_ALPHA)
                            if (useChunky) chunky = data->snchunky;
                            #endif

                            if (data->snormalBM)
                            {
                                bm = data->snormalBM;
                                mask = data->smask;
                            }
                        }
                        else
                        {
                            if (data->strip->snormalBM)
                            {
                                #if defined(WITH_ALPHA)
        	                if (useChunky) chunky = data->strip->snchunky;
                                #endif

                                bm = data->strip->snormalBM;
                                mask = data->strip->smask;
                            }
                        }
                    }
                    else
                    {
                        if (data->snormalBM)
                        {
                            #if defined(WITH_ALPHA)
    	                    if (useChunky) chunky = data->snchunky;
                            #endif

                            bm = data->snormalBM;
                            mask = data->smask;
                        }
                    }
                }
                else if(!ov && (data->flags & FLG_Sunny))
                {
                        if (strip)
                        {
        	                if (data->flags & FLG_Scaled)
                            {
                                #if defined(WITH_ALPHA)
                                if (useChunky) chunky = data->gchunky;
                                #endif

                                if (data->greyBM)
                                {
                                    bm = data->greyBM;
                                }
                            }
                            else
                            {
                                #if defined(WITH_ALPHA)
                                if (useChunky) if (data->strip->gchunky) chunky = data->strip->gchunky;
                                #endif

                                if (data->strip->greyBM) bm = data->strip->greyBM;
                            }
                        }
                        else
                        {
                            #if defined(WITH_ALPHA)
                            if (useChunky) if (data->gchunky) chunky = data->gchunky;
                            #endif

                            if (data->greyBM) bm = data->greyBM;
                        }
                }

                #if defined(WITH_ALPHA)
                if (bm || chunky)
                #else
                if (bm)
                #endif
                {
                    if (bm && mask && di && (data->disMode==MUIV_TheButton_DisMode_Grid))
                    {
                        struct BitMap *tbm;

                        if((tbm = (AllocBitMap(iw,ih,GetBitMapAttr(bm,BMA_DEPTH),(data->flags & FLG_CyberMap) ? BMF_MINPLANES : 0,(data->flags & FLG_CyberMap) ? bm : NULL))))
                        {
                            struct BitMap *tbmask = NULL; // gcc
                            APTR 	    tmask;

                            if (strip && ! (data->flags & FLG_Scaled))
                            {
                                if((tbmask = AllocBitMap(iw,ih,1,0,NULL)))
                                {
                                    struct BitMap sbm;

                                    memset(&sbm,0,sizeof(sbm));
                                    sbm.BytesPerRow = RAWIDTH(data->image->dataWidth);
                                    sbm.Rows        = data->image->dataHeight;
                                    sbm.Depth       = 1;
                                    sbm.Planes[0]   = mask;

                                    BltBitMap(&sbm,x,y,tbmask,0,0,iw,ih,0xc0,0xff,NULL);

                                    tmask = tbmask->Planes[0];
                                }
                                else tmask = NULL;
                            }
                            else tmask = mask;

                            if (tmask)
                            {
                                struct RastPort trp;
                                UWORD grid[] = { 0x5555, 0xAAAA };

                                BltBitMap(bm,x,y,tbm,0,0,iw,ih,0xc0,0xff,NULL);

                                memcpy(&trp,rp,sizeof(trp));
                                trp.Layer  = NULL;
                                trp.BitMap = tbm;

                                SetAPen(&trp,MUIPEN(data->disBodyPen));
                                SetAfPt(&trp,grid,1);

                                RectFill(&trp,0,0,iw-1,ih-1);

                                BltMaskBitMapRastPort(tbm,0,0,rp,ixp,iyp,iw,ih,(ABC|ABNC|ANBC),tmask);

                                if (tmask!=mask)
                                {
                                    WaitBlit();
                                    FreeBitMap(tbmask);
                                }

                                done = TRUE;
                            }

                            WaitBlit();
                            FreeBitMap(tbm);
                        }
                    }

                    if (!done)
                    {
                        #if defined(WITH_ALPHA)
    	                if (chunky)
                        {
	                        if (data->image->flags & BRFLG_EmpytAlpha) WritePixelArray(chunky,x,y,(data->flags & FLG_Scaled) ? iw*4 : data->image->dataWidth*4,rp,ixp,iyp,iw,ih,RECTFMT_ARGB);
        			        else WritePixelArrayAlpha(chunky,x,y,(data->flags & FLG_Scaled) ? iw*4 : data->image->dataWidth*4,rp,ixp,iyp,iw,ih,lib_alpha);
                        }
                        else
                        #endif
                        {
                            if(bm)
                            {
    	                        //if ((data->image->flags & BRFLG_EmpytAlpha) && mask)
    	                        if(mask)
                                BltMaskBitMapRastPort(bm,x,y,rp,ixp,iyp,iw,ih,(ABC|ABNC|ANBC),mask);
                        	    else
                                BltBitMapRastPort(bm,x,y,rp,ixp,iyp,iw,ih,0xc0);
                            }
                        }

                        if (di)
                        {
                            UWORD grid[] = { 0x5555, 0xAAAA };

                            SetAPen(rp,MUIPEN(data->disBodyPen));
                            SetAfPt(rp,grid,1);

                            RectFill(rp,_left(obj),_top(obj),_right(obj),_bottom(obj));

                            SetAfPt(rp,NULL,0);
                        }
                    }
                }
            }
            else
            {
                if (di && (data->disMode==MUIV_TheButton_DisMode_Grid || data->disMode==MUIV_TheButton_DisMode_FullGrid))
                {
                    UWORD grid[] = { 0x5555, 0xAAAA };

                    SetAPen(rp,MUIPEN(data->disBodyPen));
                    SetAfPt(rp,grid,1);

                    RectFill(rp,_left(obj),_top(obj),_right(obj),_bottom(obj));

                    SetAfPt(rp,NULL,0);
                }
            }
        }

        if (bl && ((se && !(data->userFlags & UFLG_NtRaiseActive)) || (ov && ra)))
        {
            ULONG dsize = data->fSize>1;
            LONG  shine, shadow;
            WORD  t = _top(obj), l = _left(obj), r = _right(obj), b = _bottom(obj);

            if (data->fStyle==MUIV_TheButton_FrameStyle_Recessed)
                if (se)
                {
                    shine  = MUIPEN(data->shine);
                    shadow = MUIPEN(data->shadow);
                }
                else
                {
                    shine  = MUIPEN(data->shadow);
                    shadow = MUIPEN(data->shine);
                }
            else
                if (se)
                {
                    shine  = MUIPEN(data->shadow);
                    shadow = MUIPEN(data->shine);
                }
                else
                {
                    shine  = MUIPEN(data->shine);
                    shadow = MUIPEN(data->shadow);
                }

            SetAPen(rp,shadow);
            Move(rp,r,t);
            Draw(rp,r,b);
            Draw(rp,l+1,b);
            if (dsize)
            {
                Move(rp,r-1,t+1);
                Draw(rp,r-1,b-1);
                //Draw(rp,l+2,b-1);
            }

            SetAPen(rp,shine);
            Move(rp,l,b);
            Draw(rp,l,t);
            Draw(rp,r-1,t);
            if (dsize)
            {
                Move(rp,l+1,b-1);
                Draw(rp,l+1,t+1);
                //Draw(rp,r-2,t+1);
            }
        }
    }

    return 0;
}

/***********************************************************************/

static ULONG
mHandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
  struct InstData *data = INST_DATA(cl,obj);

  ENTER();


  if(data->flags2 & FLG2_Special)
  {
    data->flags2 &= ~FLG2_Special;
    addRemEventHandler(cl,obj,data);
  }

  if((data->flags & FLG_Handler) && !(data->flags & FLG_IsSpacer))
  {
    if(msg->imsg)
    {
      switch(msg->imsg->Class)
      {
        case IDCMP_RAWKEY:
        case IDCMP_MOUSEBUTTONS:
        {
          // we only catch the qualifiers as the rest
          // is done by the Area MUI class
          data->qualifier = msg->imsg->Qualifier;
        }
        break;

        default:
        {
          ULONG in = checkIn(obj, data, msg->imsg->MouseX, msg->imsg->MouseY);

          if(!BOOLSAME(in, data->flags & FLG_MouseOver))
            set(obj, MUIA_TheButton_MouseOver, in);
        }
      }
    }
  }

  RETURN(0);
  return 0;
}

/***********************************************************************/

static ULONG
mNotify(struct IClass *cl, Object *obj, struct MUIP_Notify *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result = 0;

  ENTER();

  // we catch the MUIM_Notify and send our super class a different
  // modified notify method so that it informs us first and we will later
  // on (in CheckNotify) inform the original Notify destination

  // first we add the new notify to the notifyList of this button
  if(msg->FollowParams > 0)
  {
    struct ButtonNotify *notify;
    ULONG size;

    // calculate the size of the notify structure with some additional
    // space at the end to put in the parameters as well...
    size = sizeof(struct ButtonNotify)+(sizeof(ULONG)*msg->FollowParams);

    // now we allocate a new ButtonNotify and add
    // it to the notify list of the button
    if((notify = allocVecPooled(data->pool, size)))
    {
      // now we fill the notify structure
      memcpy(&notify->msg, msg, sizeof(struct MUIP_Notify)+(sizeof(ULONG)*msg->FollowParams));

      // add the new notify to the notifies list of the button
      AddTail((struct List *)&data->notifyList, (struct Node *)notify);

      // now that we have added the new notify to our list
      // we go and create a "faked" notify message we send
      result = DoSuperMethod(cl, obj, MUIM_Notify, msg->TrigAttr, msg->TrigVal, obj, 2, MUIM_TheButton_SendNotify, notify);

      // in case the DoSuperMethod returns an error we go and cleanup the notify again
      if(result == 0)
      {
        Remove((struct Node *)notify);
        freeVecPooled(data->pool, notify);
      }
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mKillNotify(struct IClass *cl, Object *obj, struct MUIP_KillNotify *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result;
  struct MinNode *node;

  ENTER();

  // now we have to walk through our notifyList and find all notifies
  // that trigger on a specific attribute and delete it.
  for(node = data->notifyList.mlh_Head; node->mln_Succ; )
  {
    struct ButtonNotify *notify = (struct ButtonNotify *)node;

    // walk on in advance so we don't invalidate the list
    node = node->mln_Succ;

    // check if the attribute of the notify
    // matches the one of this method
    if(notify->msg.TrigAttr == msg->TrigAttr)
    {
      // Remove node from list
      Remove((struct Node *)notify);

      // Free everything of the node
      freeVecPooled(data->pool, notify);

      // we walk on as there might be more than
      // one notify on an attribute.
    }
  }

  result = DoSuperMethodA(cl, obj, (APTR)msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mKillNotifyObj(struct IClass *cl, Object *obj, struct MUIP_KillNotifyObj *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  ULONG result;
  struct MinNode *node;

  ENTER();

  // now we have to walk through our notifyList and find all notifies
  // that trigger on a specific attribute and delete it.
  for(node = data->notifyList.mlh_Head; node->mln_Succ; )
  {
    struct ButtonNotify *notify = (struct ButtonNotify *)node;

    // walk on in advance so we don't invalidate the list
    node = node->mln_Succ;

    // check if the attribute and destination object of the notify
    // matches the one of this method
    if(notify->msg.TrigAttr == msg->TrigAttr &&
       notify->msg.DestObj == msg->dest)
    {
      // Remove node from list
      Remove((struct Node *)notify);

      // Free everything of the node
      freeVecPooled(data->pool, notify);

      // we walk on as there might be more than
      // one notify on an attribute.
    }
  }

  result = DoSuperMethodA(cl, obj, (APTR)msg);

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mSendNotify(struct IClass *cl, Object *obj, struct MUIP_TheButton_SendNotify *msg)
{
  struct InstData *data = INST_DATA(cl,obj);
  struct MinNode *node;
  BOOL result = FALSE;

  ENTER();

  // check if we should send notifies at all
  if(!(data->flags & FLG_NoNotify))
  {
    // now we first check if that notify is still part of our notifyList
    // and if so we directly forward it to the original destination. However,
    // we do care about the special MUIV_TheBar_Qualifier on which we put
    // the current qualifier in instead.
    for(node = data->notifyList.mlh_Head; node->mln_Succ; node = node->mln_Succ)
    {
      struct ButtonNotify *notify = (struct ButtonNotify *)node;

      if(notify == msg->notify)
      {
        ULONG *destMessage; // Msg is defined in intuition/classusr.h

        // now we create a full temporary clone of the notify
        // message which we can modify before we send it to
        // the destination
        if((destMessage = allocVecPooled(data->pool, sizeof(ULONG)*(notify->msg.FollowParams))))
        {
          ULONG i;
          Object *destObj = NULL;
          ULONG *para = (ULONG *)(((LONG)&notify->msg.FollowParams)+sizeof(ULONG));

          // now we fill the notify structure
          memcpy(destMessage, para, sizeof(ULONG)*(notify->msg.FollowParams));

          // parse through the destMessage and replace certain
          // variable with their correct values.
          switch((ULONG)notify->msg.DestObj)
          {
            case MUIV_Notify_Self:
              destObj = obj;
            break;

            case MUIV_Notify_Window:
              destObj = _win(obj);
            break;

            case MUIV_Notify_Application:
              destObj = _app(obj);
            break;

            default:
              destObj = notify->msg.DestObj;
          }

          // we have to find special Values that we need to replace
          // with the real values. But here we start with 1 as the first
          // parameter is ALWAYS the Method name and that can't be
          // replaced.
          for(i=1; i < notify->msg.FollowParams; i++)
          {
            switch(destMessage[i])
            {
              case MUIV_TriggerValue:
                destMessage[i] = notify->msg.TrigVal;
              break;

              case MUIV_NotTriggerValue:
                destMessage[i] = !notify->msg.TrigVal;
              break;

              case MUIV_TheBar_Qualifier:
                destMessage[i] = data->qualifier;
              break;
            }
          }

          // send the destMessage to the object
          if(destObj)
          {
            DoMethodA(destObj, (Msg)destMessage);
            result = TRUE;
          }

          // cleanup the temporary clone.
          freeVecPooled(data->pool, destMessage);
        }

        // break out
        break;
      }
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/

static ULONG
mCustomBackfill(struct IClass *cl,Object *obj,struct MUIP_CustomBackfill *msg)
{
    struct InstData *data = INST_DATA(cl,obj);

    //if (lib_flags & BASEFLG_MUI20) return DoSuperMethodA(cl,obj,(Msg)msg);


    if (data->tb &&
        ((data->flags & FLG_IsSpacer) ||
         ((data->flags & FLG_Raised) && (!(data->flags & FLG_MouseOver) || (data->flags & FLG_Selected) || !data->activeBack))||
         !(data->flags & FLG_Raised)))
    {
        DoMethod(data->tb,MUIM_CustomBackfill,
            msg->left,
            msg->top,
            msg->right,
            msg->bottom,
            msg->left+msg->xoffset,
            msg->top+msg->yoffset,
            0);
    }
    else
    {
        DoSuperMethod(cl,obj,MUIM_DrawBackground,msg->left,msg->top,msg->right-msg->left+1,msg->bottom-msg->top+1,msg->xoffset,msg->yoffset,0);
    }

    return 0;
}

/***********************************************************************/

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR,_Dispatcher,cl,obj,msg)
#else
DISPATCHER(_Dispatcher)
#endif
{
  switch(msg->MethodID)
  {
    case OM_NEW:                     return mNew(cl, obj, (APTR)msg);
    case OM_DISPOSE:                 return mDispose(cl, obj, (APTR)msg);
    case OM_GET:                     return mGet(cl, obj, (APTR)msg);
    case OM_SET:                     return mSets(cl, obj, (APTR)msg);

    case MUIM_Draw:                  return mDraw(cl, obj, (APTR)msg);
    case MUIM_CustomBackfill:        return mCustomBackfill(cl, obj, (APTR)msg);
    case MUIM_HandleEvent:           return mHandleEvent(cl, obj, (APTR)msg);
    case MUIM_AskMinMax:             return mAskMinMax(cl, obj, (APTR)msg);
    case MUIM_Setup:                 return mSetup(cl, obj, (APTR)msg);
    case MUIM_Cleanup:               return mCleanup(cl, obj, (APTR)msg);
    case MUIM_Show:                  return mShow(cl, obj, (APTR)msg);
    case MUIM_Hide:                  return mHide(cl, obj, (APTR)msg);

    case MUIM_Notify:                return mNotify(cl, obj, (APTR)msg);
    case MUIM_KillNotify:            return mKillNotify(cl, obj, (APTR)msg);
    case MUIM_KillNotifyObj:         return mKillNotifyObj(cl, obj, (APTR)msg);

    case MUIM_TheButton_Build:       return mBuild(cl, obj, (APTR)msg);
    case MUIM_TheButton_SendNotify:  return mSendNotify(cl, obj, (APTR)msg);

    default:
      return DoSuperMethodA(cl, obj, msg);
  }
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

/***********************************************************************/
