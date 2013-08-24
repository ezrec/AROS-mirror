/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

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
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>
#include <libraries/asl.h>

/***********************************************************************/

static struct MUI_CustomClass *patterns = NULL;
#define patternsObject NewObject(patterns->mcc_Class,NULL

static struct MUI_CustomClass *dtpic = NULL;
#define dtpicObject NewObject(dtpic->mcc_Class,NULL

static struct MUI_CustomClass *bitmap = NULL;
#define bitmapObject NewObject(bitmap->mcc_Class,NULL

static struct MUI_CustomClass *gradientfield;
#define gradientfieldObject NewObject(gradientfield->mcc_Class,NULL

static struct MUI_CustomClass *gradient = NULL;
#define gradientObject NewObject(gradient->mcc_Class,NULL

/***********************************************************************/

struct patternsData
{
    Object                      *pop;
    Object                      *back;
    Object                      *patterns[16];
    char                        specs[18][16];
    LONG                        pattern;
    struct MUI_EventHandlerNode eh;
    ULONG                       secs;
    ULONG                       micros;
};

/***********************************************************************/

static IPTR
mPatternsNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *pgroup;

    if((obj = (Object *)DoSuperNew(cl,obj,
            Child, owspace(1),
            Child, HGroup,
                Child, owspace(1),
                Child, (IPTR)(pgroup = (Object *)RowGroup(3), End),
                Child, owspace(1),
            End,
            Child, owspace(1),
            TAG_MORE, msg->ops_AttrList)))
    {
        struct patternsData *data = INST_DATA(cl,obj);
        int                 i;

        for (i = 0; i<18; i++)
        {
            snprintf(data->specs[i], sizeof(data->specs[i]), (STRPTR)"0:%d", (int)(i+MUII_BACKGROUND));

            data->patterns[i] = (Object *)ImageObject,
                ButtonFrame,
                MUIA_Background,      MUII_ButtonBack,
                MUIA_InputMode,       MUIV_InputMode_Immediate,
                MUIA_InnerLeft,       4,
                MUIA_InnerTop,        4,
                MUIA_InnerRight,      4,
                MUIA_InnerBottom,     4,
                MUIA_FixWidth,        24,
                MUIA_FixHeight,       24,
                MUIA_Image_FreeHoriz, TRUE,
                MUIA_Image_FreeVert,  TRUE,
                MUIA_Image_Spec,      (IPTR)data->specs[i],
            End;

            if (data->patterns[i])
            {
                DoMethod(pgroup,OM_ADDMEMBER,data->patterns[i]);
                DoMethod(data->patterns[i],MUIM_Notify,MUIA_Selected,TRUE,obj,2,MUIM_Popbackground_SelectPattern,i);
            }
            else
            {
                CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
                return 0;
            }
        }

        data->pop  = (Object *)GetTagData(MUIA_Popbackground_PopObj, 0, msg->ops_AttrList);
        data->back = (Object *)GetTagData(MUIA_Popbackground_BackObj, 0, msg->ops_AttrList);
        data->pattern = -1;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mPatternsShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct patternsData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    memset(&data->eh,0,sizeof(data->eh));
    data->eh.ehn_Class  = cl;
    data->eh.ehn_Object = obj;
    data->eh.ehn_Events = IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY;
    data->eh.ehn_Flags  = MUI_EHF_ALWAYSKEYS;
    DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->eh);

    return TRUE;
}

/***********************************************************************/

static IPTR
mPatternsHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct patternsData *data = INST_DATA(cl,obj);

    DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->eh);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mPatternsHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    if (msg->imsg)
    {
        struct patternsData *data = INST_DATA(cl,obj);
        LONG                p = data->pattern;

        if (msg->imsg->Class==IDCMP_MOUSEBUTTONS)
        {
            Object *o = data->patterns[p];
            UWORD  mx = msg->imsg->MouseX, my = msg->imsg->MouseY;

            if ((p>=0) &&
                 msg->imsg->Qualifier==0xc000 &&
                _left(o)<mx && _right(o)>mx && _top(o)<my && _bottom(o)>my &&
                DoubleClick(data->secs,data->micros,msg->imsg->Seconds,msg->imsg->Micros))
                {
                    DoMethod(_app(obj),MUIM_Application_PushMethod,data->pop,2,MUIM_Popbackground_Close,TRUE);
                }
        }
        else
        {
            Object *o;

            get(_win(obj),MUIA_Window_ActiveObject,&o);
            if (o) return DoSuperMethodA(cl,obj,(Msg)msg);

            switch (msg->muikey)
            {
                case MUIKEY_UP:
                    if (p<0) p = 0;
                    else if (p-6>=0) p -= 6;
                         else if (p==0) p = 17;
                              else p += 11;
                    break;

                case MUIKEY_DOWN:
                    if (p<0) p = 0;
                    else if (p+6<=17) p += 6;
                         else if (p==17) p = 0;
                              else p -= 11;
                    break;

                case MUIKEY_LEFT:
                    if (p<0) p = 0;
                    else if (p-1>=0) p -= 1;
                         else p = 17;
                    break;

                case MUIKEY_RIGHT:
                    if (p<0) p = 0;
                    else if (p+1<=17) p += 1;
                         else p = 0;
                    break;

                case MUIKEY_PRESS:
                    if (p>=0)
                    {
                        DoMethod(_app(obj),MUIM_Application_PushMethod,data->pop,2,MUIM_Popbackground_Close,TRUE);
                    }
                    break;
            }

            if (p!=data->pattern)
            {
                set(data->patterns[p],MUIA_Selected,TRUE);
            }
        }

        data->secs   = msg->imsg->Seconds;
        data->micros = msg->imsg->Micros;
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mPatternsSetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_SetSpec *msg)
{
    struct patternsData *data = INST_DATA(cl,obj);
    char                *spec = msg->spec;
    LONG        v;

    if (!spec || (*spec++!='0') || (!*spec++==':') || !*spec) return MUIV_Popbackground_SetSpec_Fail;

    v = atoi(spec)-MUII_BACKGROUND;
    if (v<0 || v>17) return FALSE;

    set(data->patterns[v],MUIA_Selected,TRUE);

    return MUIV_Popbackground_SetSpec_Spec;
}

/***********************************************************************/

static IPTR
mPatternsGetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_GetSpec *msg)
{
    struct patternsData *data = INST_DATA(cl,obj);

    if (data->pattern>=0)
    {
        sprintf(msg->spec, (STRPTR)"0:%ld", data->pattern+MUII_BACKGROUND);
        return MUIV_Popbackground_GetSpec_Spec;
    }

    *msg->spec = 0;
    return MUIV_Popbackground_GetSpec_Fail;
}

/***********************************************************************/

static IPTR
mPatternsSelectPattern(struct IClass *cl,Object *obj,struct MUIP_Popbackground_SelectPattern *msg)
{
    struct patternsData *data = INST_DATA(cl,obj);
    IPTR x;

    if((data->pattern >= 0) && (data->pattern != (LONG)msg->id))
        set(data->patterns[data->pattern],MUIA_Selected,FALSE);

    get(data->patterns[msg->id],MUIA_Selected,&x);
    if (!x) nnset(data->patterns[msg->id],MUIA_Selected,TRUE);

    data->pattern = msg->id;

    return 0;
}

/***********************************************************************/

DISPATCHER(patternsDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                           return mPatternsNew(cl,obj,(APTR)msg);
    case MUIM_Show:                        return mPatternsShow(cl,obj,(APTR)msg);
    case MUIM_Hide:                        return mPatternsHide(cl,obj,(APTR)msg);
    case MUIM_HandleEvent:                 return mPatternsHandleEvent(cl,obj,(APTR)msg);
    case MUIM_Popbackground_SetSpec:       return mPatternsSetSpec(cl,obj,(APTR)msg);
    case MUIM_Popbackground_GetSpec:       return mPatternsGetSpec(cl,obj,(APTR)msg);
    case MUIM_Popbackground_SelectPattern: return mPatternsSelectPattern(cl,obj,(APTR)msg);
    default:                               return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static void
freePatterns(void)
{
    if (patterns)
    {
        MUI_DeleteCustomClass(patterns);
        patterns = NULL;
    }
}

/***********************************************************************/

static ULONG
initPatterns(void)
{
  if((patterns = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct patternsData), ENTRY(patternsDispatcher))))
  {
    if(lib_flags & BASEFLG_MUI20)
      patterns->mcc_Class->cl_ID = (STRPTR)"Backgroundadjust_Patterns";

    return TRUE;
  }

  return FALSE;
}

/***********************************************************************/

struct dtpicData
{
    char                file[256];
    Object              *dto;
    struct BitMapHeader *bmhd;
    struct BitMap       *bitmap;
    APTR                plane;
    ULONG               flags;
};

enum
{
    FLG_Setup       = 1<<0,
};

/***********************************************************************/

static void
freeDTO(struct dtpicData *data)
{
    data->bitmap = NULL;
    data->bmhd   = NULL;

    if (data->dto)
    {
        DisposeDTObject(data->dto);
        data->dto = NULL;
    }
}

/***********************************************************************/

static BOOL
build(UNUSED struct IClass *cl,Object *obj,struct dtpicData *data)
{
    freeDTO(data);

    if ((data->flags & FLG_Setup) && *data->file)
    {
        struct Process *me = (struct Process *)FindTask(NULL);
        APTR           win = me->pr_WindowPtr;
        ULONG          cyber = CyberGfxBase && IsCyberModeID(GetVPModeID(&_screen(obj)->ViewPort));

        me->pr_WindowPtr = (APTR)-1;

        data->dto = NewDTObject(data->file,DTA_GroupID,                        GID_PICTURE,
                                           PDTA_Screen,                        _screen(obj),
                                           PDTA_FreeSourceBitMap,              TRUE,
                                           PDTA_UseFriendBitMap,               cyber,
                                           cyber ? PDTA_DestMode : TAG_IGNORE, PMODE_V43,
                                           TAG_DONE);

        me->pr_WindowPtr = win;

        if (data->dto)
        {
            struct FrameInfo fri;

            memset(&fri, 0, sizeof(struct FrameInfo));

            DoMethod(data->dto,DTM_FRAMEBOX,NULL,&fri,&fri,sizeof(struct FrameInfo),0);

            if (fri.fri_Dimensions.Depth>0)
            {
                if (DoMethod(data->dto,DTM_PROCLAYOUT,NULL,TRUE))
                {
                    get(data->dto,PDTA_BitMapHeader,&data->bmhd);

                    if (data->bmhd)
                    {
                        GetDTAttrs(data->dto,PDTA_DestBitMap,&data->bitmap,TAG_DONE);
                        if (!data->bitmap) GetDTAttrs(data->dto,PDTA_BitMap,&data->bitmap,TAG_DONE);
                        if (data->bitmap)
                        {
                            IPTR storage;

                            GetDTAttrs(data->dto,PDTA_MaskPlane,&storage,TAG_DONE);
                            data->plane = (APTR)storage;

                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    freeDTO(data);

    return FALSE;
}

/***********************************************************************/

static IPTR
mDTPicNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
    {
        msg->MethodID = OM_SET;
        DoMethodA(obj,(Msg)msg);
        msg->MethodID = OM_NEW;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mDTPicDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct dtpicData *data = INST_DATA(cl, obj);

    if (data->dto) DisposeDTObject(data->dto);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR
mDTPicSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct dtpicData *data = INST_DATA(cl, obj);
    struct TagItem *tags;
    struct TagItem *tag;
    BOOL rebuild, redraw;
    IPTR  res;

    rebuild = redraw = FALSE;

    for(tags = msg->ops_AttrList; (tag = NextTagItem((APTR)&tags)); )
    {
        IPTR tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_Popbackground_File:
            {
                if(tidata)
                  strlcpy(data->file, (STRPTR)tidata, sizeof(data->file));
                else
                  *data->file = '\0';

                rebuild = TRUE;
            }
            break;
        }
    }

    res = DoSuperMethodA(cl,obj,(Msg)msg);

    if (data->flags & FLG_Setup)
    {
        if (rebuild)
        {
            Object                 *parent;
            struct BitMap *bm = data->bitmap;

            set(_app(obj),MUIA_Application_Sleep,TRUE);

            if (bm)
            {
                data->bitmap = NULL;
                MUI_Redraw(obj,MADF_DRAWOBJECT);
                data->bitmap = bm;
            }

            superget(cl,obj,MUIA_Parent,&parent);
            if (parent) DoMethod(parent,MUIM_Group_InitChange);

            build(cl,obj,data);

            if (parent) DoMethod(parent,MUIM_Group_ExitChange);

            MUI_Redraw(obj,MADF_DRAWOBJECT);

            set(_app(obj),MUIA_Application_Sleep,FALSE);
        }
        else
            if (redraw) MUI_Redraw(obj,MADF_DRAWOBJECT);
    }

    return res;
}

/***************************************************************************/

static IPTR
mDTPicSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct dtpicData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg)) return FALSE;

    data->flags |= FLG_Setup;
    build(cl,obj,data);

    return TRUE;
}

/***********************************************************************/

static IPTR
mDTPicCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct dtpicData *data = INST_DATA(cl,obj);

    freeDTO(data);
    data->flags &= ~FLG_Setup;

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mDTPicAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct dtpicData    *data = INST_DATA(cl,obj);
    struct MUI_MinMax *mi;

    DoSuperMethodA(cl,obj,(Msg)msg);

    mi = msg->MinMaxInfo;

    if (data->bitmap)
    {
        mi->MinWidth  += data->bmhd->bmh_Width;
        mi->MinHeight += data->bmhd->bmh_Height;
        mi->DefWidth  += data->bmhd->bmh_Width;
        mi->DefHeight += data->bmhd->bmh_Height;
    }

    mi->MaxWidth  += MUI_MAXMAX;
    mi->MaxHeight += MUI_MAXMAX;

    return 0;
}

/***********************************************************************/

static IPTR
mDTPicDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct dtpicData *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(Msg)msg);

    if ((msg->flags & MADF_DRAWOBJECT) && data->bitmap)
    {
        WORD l, t, w, h, bw, bh;

        w = _mwidth(obj);
        h = _mheight(obj);
        l = _mleft(obj);
        t = _mtop(obj);

        bw = data->bmhd->bmh_Width;
        bh = data->bmhd->bmh_Height;

        if (bw<w) w = bw;
        if (bh<h) h = bh;

        BltBitMapRastPort(data->bitmap,0,0,_rp(obj),l,t,w,h,0xc0);
    }

    return 0;
}

/***********************************************************************/

DISPATCHER(dtpicDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW:         return mDTPicNew(cl,obj,(APTR)msg);
    case OM_DISPOSE:     return mDTPicDispose(cl,obj,(APTR)msg);
    case OM_SET:         return mDTPicSets(cl,obj,(APTR)msg);
    case MUIM_Draw:      return mDTPicDraw(cl,obj,(APTR)msg);
    case MUIM_AskMinMax: return mDTPicAskMinMax(cl,obj,(APTR)msg);
    case MUIM_Setup:     return mDTPicSetup(cl,obj,(APTR)msg);
    case MUIM_Cleanup:   return mDTPicCleanup(cl,obj,(APTR)msg);
    default:             return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static void
freeDTPic(void)
{
    if (dtpic)
    {
        MUI_DeleteCustomClass(dtpic);
        dtpic = NULL;
    }

}

/***********************************************************************/

static ULONG
initDTPic(void)
{
  if((dtpic = MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct dtpicData), ENTRY(dtpicDispatcher))))
  {
    if(lib_flags & BASEFLG_MUI20)
      dtpic->mcc_Class->cl_ID = (STRPTR)"Backgroundadjust_dtpic";

    return TRUE;
  }

  return FALSE;
}

/***********************************************************************/

struct bitmapData
{
    Object *bitmap;
    Object *pic;
};

/***********************************************************************/

static IPTR
mBitmapNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *pic, *bitmap, *abt, *pbt;

    if((obj = (Object *)DoSuperNew(cl,obj,
            Child, ScrollgroupObject,
                MUIA_Scrollgroup_Contents, VirtgroupObject,
                    MUIA_Frame, MUIV_Frame_Virtual,
                    Child, (IPTR)(pic = (Object *)dtpicObject, End),
                End,
            End,
            Child, HGroup,
                MUIA_Group_HorizSpacing, 1,
                Child, PopaslObject,
                    MUIA_Popstring_String, (IPTR)(bitmap = (Object *)StringObject,
                        MUIA_CycleChain,   TRUE,
                        StringFrame,
                        MUIA_String_MaxLen, 256,
                    End),
                    MUIA_Popstring_Button, (IPTR)(abt = PopButton(MUII_PopFile)),
                    MUIA_Popasl_Type,      ASL_FileRequest,
                End,
                Child, (IPTR)(pbt = PopButton(MUII_PopUp)),
            End,
            TAG_MORE, msg->ops_AttrList)))
    {
        struct bitmapData *data = INST_DATA(cl,obj);

        data->pic    = pic;
        data->bitmap = bitmap;

        DoSuperMethod(cl,obj,MUIM_MultiSet,MUIA_CycleChain,TRUE,abt,pbt,NULL);
        DoMethod(pbt,MUIM_Notify,MUIA_Pressed,FALSE,obj,1,MUIM_Popbackground_ShowBitmap);
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mBitmapSetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_SetSpec *msg)
{
    struct bitmapData *data = INST_DATA(cl,obj);
    char              *spec = msg->spec;

    if (!spec || (*spec++!='5') || (!*spec++==':') || !*spec) return MUIV_Popbackground_SetSpec_Fail;

    set(data->pic,MUIA_Popbackground_File,NULL);
    set(data->bitmap,MUIA_String_Contents,spec);
    return MUIV_Popbackground_SetSpec_Spec;
}

/***********************************************************************/

static IPTR
mBitmapGetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_GetSpec *msg)
{
    struct bitmapData *data = INST_DATA(cl,obj);
    STRPTR                      x;

    get(data->bitmap,MUIA_String_Contents,&x);
    sprintf(msg->spec,(STRPTR)"5:%s",x);
    return MUIV_Popbackground_GetSpec_Spec;
}

/***********************************************************************/

static IPTR
mBitmapShowBitmap(struct IClass *cl,Object *obj, UNUSED Msg msg)
{
    struct bitmapData *data = INST_DATA(cl,obj);
    STRPTR                     x;

    get(data->bitmap,MUIA_String_Contents,&x);
    set(data->pic,MUIA_Popbackground_File,x);

    return 0;
}

/***********************************************************************/

DISPATCHER(bitmapDispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:                        return mBitmapNew(cl,obj,(APTR)msg);
        case MUIM_Popbackground_SetSpec:    return mBitmapSetSpec(cl,obj,(APTR)msg);
        case MUIM_Popbackground_GetSpec:    return mBitmapGetSpec(cl,obj,(APTR)msg);
        case MUIM_Popbackground_ShowBitmap: return mBitmapShowBitmap(cl,obj,(APTR)msg);
        default:                            return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

static void
freeBitmap(void)
{
    freeDTPic();

    if (bitmap)
    {
        MUI_DeleteCustomClass(bitmap);
        bitmap = NULL;
    }
}

/***********************************************************************/

static ULONG
initBitmap(void)
{
    if (initDTPic() &&
        (bitmap = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct bitmapData),ENTRY(bitmapDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            bitmap->mcc_Class->cl_ID = (STRPTR)"Backgroundadjust_Bitmap";

        return TRUE;
    }

    freeBitmap();

    return FALSE;
}

/***********************************************************************/

struct gradientFieldData
{
    struct MUIS_TheBar_Gradient grad;
    ULONG                       flags;
};

enum
{
    FLG_CyberDeep  = 1<<0,
};

/***********************************************************************/

static IPTR
mGradientFieldNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_InnerLeft,   2,
            MUIA_InnerTop,    2,
            MUIA_InnerRight,  2,
            MUIA_InnerBottom, 2,
            TAG_MORE,msg->ops_AttrList)))
    {
        struct gradientFieldData *data = INST_DATA(cl,obj);

        if (GetTagData(MUIA_Popbackground_Horiz,TRUE,msg->ops_AttrList))
            data->grad.flags |= MUIV_TheBar_Gradient_Horiz;

        data->grad.from = 0xffffff;
        data->grad.to   = 0xffffff;
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGradientFieldSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct gradientFieldData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    BOOL redraw = FALSE;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_Popbackground_From:
                if (data->grad.from!=tidata)
                {
                    data->grad.from = tidata;
                    redraw = TRUE;
                }
                break;

            case MUIA_Popbackground_To:
                if (data->grad.to!=tidata)
                {
                    data->grad.to = tidata;
                    redraw = TRUE;
                }
                break;
        }
    }

    if (redraw) MUI_Redraw(obj,MADF_DRAWUPDATE);

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mGradientFieldSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct gradientFieldData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg)) return FALSE;

    if ((GetBitMapAttr(_screen(obj)->RastPort.BitMap,BMA_DEPTH)>8) &&
        CyberGfxBase && IsCyberModeID(GetVPModeID(&_screen(obj)->ViewPort)))
        data->flags |= FLG_CyberDeep;
    else data->flags &= ~FLG_CyberDeep;

    return TRUE;
}

/***********************************************************************/

static IPTR
mGradientFieldDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct gradientFieldData *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(Msg)msg);

    if ((data->flags & FLG_CyberDeep) && (msg->flags & (MADF_DRAWUPDATE|MADF_DRAWOBJECT)))
        drawGradient(obj,&data->grad);

    return 0;
}

/***********************************************************************/

DISPATCHER(gradientFieldDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:     return mGradientFieldNew(cl,obj,(APTR)msg);
    case OM_SET:     return mGradientFieldSets(cl,obj,(APTR)msg);
    case MUIM_Setup: return mGradientFieldSetup(cl,obj,(APTR)msg);
    case MUIM_Draw:  return mGradientFieldDraw(cl,obj,(APTR)msg);
    default:         return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static void
freeGradientfield(void)
{
    if (gradientfield)
    {
        MUI_DeleteCustomClass(gradientfield);
        gradientfield = NULL;
    }
}

/***********************************************************************/

static ULONG
initGradientfield(void)
{
  if((gradientfield = MUI_CreateCustomClass(NULL, MUIC_Rectangle, NULL, sizeof(struct gradientFieldData), ENTRY(gradientFieldDispatcher))))
  {
    if(lib_flags & BASEFLG_MUI20)
      gradientfield->mcc_Class->cl_ID = (STRPTR)"Gradientfield";

    return TRUE;
  }

  return FALSE;
}

/***********************************************************************/

struct gradientData
{
    Object *from;
    Object *to;
    Object *hgrad;
    Object *vgrad;
};

/***********************************************************************/

static IPTR
mGradientNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *vgrad, *hgrad, *from, *to, *swap;

    if((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Group_Horiz, TRUE,

            Child, vgrad = (Object *)gradientfieldObject,
                MUIA_ShortHelp,           (IPTR)tr(Msg_Popback_VertGradientHelp),
                ButtonFrame,
                MUIA_Background,          MUII_ButtonBack,
                MUIA_InputMode,           MUIV_InputMode_Immediate,
                MUIA_FixWidth,            24,
                MUIA_Popbackground_Horiz, FALSE,
            End,

            Child, VGroup,
                Child, hgrad = (Object *)gradientfieldObject,
                    MUIA_ShortHelp,  (IPTR)tr(Msg_Popback_HorizGradientHelp),
                    ButtonFrame,
                    MUIA_Background, MUII_ButtonBack,
                    MUIA_InputMode,  MUIV_InputMode_Immediate,
                    MUIA_FixHeight,  24,
                    MUIA_Selected,   TRUE,
                End,

                Child, HGroup,
                    Child, from = coloradjustObject, MUIA_Coloradj_Colorfield, NULL, End,
                    Child, RectangleObject, MUIA_Weight, 0, MUIA_Rectangle_VBar, TRUE, End,
                    Child, to = coloradjustObject, MUIA_Coloradj_Colorfield, NULL, End,
                End,

                Child, HGroup,
                    Child, HSpace(0),
                    Child, oclabel(Msg_Popback_From),
                    Child, HSpace(0),
                    Child, swap = obutton(Msg_Popback_Swap,Msg_Popback_SwapHelp),
                    Child, HSpace(0),
                    Child, oclabel(Msg_Popback_To),
                    Child, HSpace(0),
                End,
            End,
            TAG_MORE, msg->ops_AttrList)))
    {
        struct gradientData *data = INST_DATA(cl,obj);

        data->from  = from;
        data->to    = to;
        data->hgrad = hgrad;
        data->vgrad = vgrad;

        set(swap,MUIA_Weight,20);
        DoMethod(swap,MUIM_Notify,MUIA_Pressed,FALSE,obj,1,MUIM_Popbackground_Swap);
        DoMethod(from,MUIM_Notify,MUIA_Coloradjust_RGB,MUIV_EveryTime,obj,2,MUIM_Popbackground_GradientCol,TRUE);
        DoMethod(to,MUIM_Notify,MUIA_Coloradjust_RGB,MUIV_EveryTime,obj,2,MUIM_Popbackground_GradientCol,FALSE);

        DoMethod(hgrad,MUIM_Notify,MUIA_Selected,TRUE,vgrad,3,MUIM_Set,MUIA_Selected,FALSE);
        DoMethod(vgrad,MUIM_Notify,MUIA_Selected,TRUE,hgrad,3,MUIM_Set,MUIA_Selected,FALSE);
    }

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGradientSetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_SetSpec *msg)
{
    struct gradientData *data = INST_DATA(cl,obj);
    struct MUIS_TheBar_Gradient  *grad = msg->grad;
    struct MUI_RGBcolor          rgb1, rgb2;
                        ULONG x;

    if (!grad) return MUIV_Popbackground_SetSpec_Fail;

    x = grad->from>>16;
    rgb1.red = (x<<24)|(x<<16)|(x<<8)|x;
    x = (grad->from>>8) & 0xff;
    rgb1.green = (x<<24)|(x<<16)|(x<<8)|x;
    x = grad->from & 0xff;
    rgb1.blue = (x<<24)|(x<<16)|(x<<8)|x;

    x = grad->to>>16;
    rgb2.red = (x<<24)|(x<<16)|(x<<8)|x;
    x = (grad->to>>8) & 0xff;
    rgb2.green = (x<<24)|(x<<16)|(x<<8)|x;
    x = grad->to & 0xff;
    rgb2.blue = (x<<24)|(x<<16)|(x<<8)|x;

    set(data->from,MUIA_Coloradjust_RGB,&rgb1);
    set(data->to,MUIA_Coloradjust_RGB,&rgb2);

    if (grad->flags & MUIV_TheBar_Gradient_Horiz) set(data->hgrad,MUIA_Selected,TRUE);
    else set(data->vgrad,MUIA_Selected,TRUE);

    return MUIV_Popbackground_SetSpec_Grad;
}

/***********************************************************************/

static IPTR
mGradientGetSpec(struct IClass *cl,Object *obj,struct MUIP_Popbackground_GetSpec *msg)
{
    struct gradientData *data = INST_DATA(cl,obj);
    struct MUIS_TheBar_Gradient  *grad = msg->grad;
    struct MUI_RGBcolor          *rgb1, *rgb2;
    IPTR                          x;

    get(data->from,MUIA_Coloradjust_RGB,&rgb1);
    get(data->to,MUIA_Coloradjust_RGB,&rgb2);

    grad->from = ((rgb1->red>>24)<<16)|((rgb1->green>>24)<<8)|(rgb1->blue>>24);
    grad->to   = ((rgb2->red>>24)<<16)|((rgb2->green>>24)<<8)|(rgb2->blue>>24);

    get(data->hgrad,MUIA_Selected,&x);
    if (x) grad->flags |= MUIV_TheBar_Gradient_Horiz;
    else grad->flags &= ~MUIV_TheBar_Gradient_Horiz;

    return MUIV_Popbackground_GetSpec_Grad;
}

/***********************************************************************/

static IPTR
mGradientGradientCol(struct IClass *cl,Object *obj,struct MUIP_Popbackground_GradientCol *msg)
{
    struct gradientData *data = INST_DATA(cl,obj);
    struct MUI_RGBcolor          *rgb;
    Object              *c;
    Tag                 tag;
    ULONG               col;

    if (msg->from)
    {
        c   = data->from;
        tag = MUIA_Popbackground_From;
    }
    else
    {
        c   = data->to;
        tag = MUIA_Popbackground_To;
    }

    get(c,MUIA_Coloradjust_RGB,&rgb);
    col = ((rgb->red>>24)<<16)|((rgb->green>>24)<<8)|(rgb->blue>>24);

    set(data->hgrad,tag,col);
    set(data->vgrad,tag,col);

    return 0;
}

/***********************************************************************/

static IPTR
mGradientSwap(struct IClass *cl,Object *obj, UNUSED Msg msg)
{
    struct gradientData *data = INST_DATA(cl,obj);
    struct MUI_RGBcolor          *rgb1, *rgb2, rgb;

    get(data->from,MUIA_Coloradjust_RGB,&rgb1);
    get(data->to,MUIA_Coloradjust_RGB,&rgb2);

    memcpy(&rgb,rgb1,sizeof(rgb));

    set(data->from,MUIA_Coloradjust_RGB,rgb2);
    set(data->to,MUIA_Coloradjust_RGB,&rgb);

    return 0;
}

/***********************************************************************/

DISPATCHER(gradientDispatcher)
{
  switch (msg->MethodID)
  {
    case OM_NEW:                         return mGradientNew(cl,obj,(APTR)msg);
    case MUIM_Popbackground_GradientCol: return mGradientGradientCol(cl,obj,(APTR)msg);
    case MUIM_Popbackground_Swap:        return mGradientSwap(cl,obj,(APTR)msg);
    case MUIM_Popbackground_SetSpec:     return mGradientSetSpec(cl,obj,(APTR)msg);
    case MUIM_Popbackground_GetSpec:     return mGradientGetSpec(cl,obj,(APTR)msg);
    default:                             return DoSuperMethodA(cl,obj,msg);
  }
}

/***********************************************************************/

static void
freeGradient(void)
{
    freeGradientfield();

    if (gradient)
    {
        MUI_DeleteCustomClass(gradient);
        gradient = NULL;
    }
}

/***********************************************************************/

static ULONG
initGradient(void)
{
    if (initGradientfield() &&
        (gradient = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct gradientData), ENTRY(gradientDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            gradient->mcc_Class->cl_ID = (STRPTR)"Gradient";

        return TRUE;
    }

    freeGradient();

    return FALSE;
}

/***********************************************************************/

enum
{
    PAGE_Pattern,
    PAGE_Color,
    PAGE_Bitmap,
    PAGE_Gradient,
};

/***********************************************************************/

struct backData
{
    Object                      *pages[8];
    ULONG                       page;
    char                        spec[POPBACKGROUNDSPECSIZE];
    struct MUIS_TheBar_Gradient grad;
    ULONG                       flags;
};

enum
{
    FLG_Gradient     = 1<<0,
    FLG_GradientMode = 1<<1,
};

/***********************************************************************/

static IPTR
mBackNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct backData *data;
    Object          *pop;
    const char **t;
    ULONG           useGradient;

    if (GetTagData(MUIA_Popbackground_Gradient,FALSE,msg->ops_AttrList))
    {
      static const char *titles[5];

      titles[0] = tr(Msg_Popback_Pattern);
      titles[1] = tr(Msg_Popback_Color);
      titles[2] = tr(Msg_Popback_Bitmap);
      titles[3] = tr(Msg_Popback_Gradient);
      titles[4] = NULL;

      useGradient = TRUE;
      t = titles;
    }
    else
    {
      static const char *ngtitles[4];

      ngtitles[0] = tr(Msg_Popback_Pattern);
      ngtitles[1] = tr(Msg_Popback_Color);
      ngtitles[2] = tr(Msg_Popback_Bitmap);
      ngtitles[3] = NULL;

      useGradient = FALSE;
      t = ngtitles;
    }

    if (!(obj = (Object *)DoSuperNew(cl,obj,MUIA_Register_Titles,t,MUIA_CycleChain,TRUE,TAG_MORE,msg->ops_AttrList)))
        return 0;

    data = INST_DATA(cl,obj);

    pop = (Object *)GetTagData(MUIA_Popbackground_PopObj, (IPTR)NULL, msg->ops_AttrList);

    if (!(data->pages[0] = patternsObject, MUIA_Popbackground_PopObj, pop, MUIA_Popbackground_BackObj, obj, End))
    {
        CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
        return 0;
    }
    DoMethod(obj,OM_ADDMEMBER,data->pages[0]);

    if (!(data->pages[1] = penadjustObject, MUIA_Popbackground_PopObj, pop, MUIA_Popbackground_BackObj, obj, End))
    {
        CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
        return 0;
    }
    DoMethod(obj,OM_ADDMEMBER,data->pages[1]);

    if (!(data->pages[2] = bitmapObject, MUIA_Popbackground_PopObj, pop, MUIA_Popbackground_BackObj, obj, End))
    {
        CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
        return 0;
    }
    DoMethod(obj,OM_ADDMEMBER,data->pages[2]);

    if (useGradient)
    {
        if (!(data->pages[3] = gradientObject, MUIA_Popbackground_PopObj, pop, MUIA_Popbackground_BackObj, obj, End))
        {
            CoerceMethod(cl->cl_Super,obj,OM_DISPOSE);
            return 0;
        }
        DoMethod(obj,OM_ADDMEMBER,data->pages[3]);

        data->flags |= FLG_Gradient;
    }

    data->page = 0;

    msg->MethodID = OM_SET;
    DoMethodA(obj,(Msg)msg);
    msg->MethodID = OM_NEW;

    return (IPTR)obj;
}

/***********************************************************************/

static ULONG
specToGadgets(struct IClass *cl,Object *obj,STRPTR spec,struct MUIS_TheBar_Gradient *grad)
{
    struct backData *data = INST_DATA(cl,obj);
    int i;

    for (i = 0; data->pages[i]; i++)
    {
        switch(DoMethod(data->pages[i],MUIM_Popbackground_SetSpec,spec,grad,MUIV_Popbackground_SetSpec_Image))
        {
            case MUIV_Popbackground_SetSpec_Fail:
                continue;

            case MUIV_Popbackground_SetSpec_Spec:
            {
                strlcpy(data->spec, spec, sizeof(data->spec));
                set(obj,MUIA_Group_ActivePage,i);
                data->flags &= ~FLG_GradientMode;

                return TRUE;
            }

            case MUIV_Popbackground_SetSpec_Grad:
            {
                memcpy(&data->grad,grad,sizeof(data->grad));
                set(obj,MUIA_Group_ActivePage,i);
                data->flags |= FLG_GradientMode;

                return TRUE;
            }
        }
    }

    return FALSE;
}

/***********************************************************************/

static void
gadgetsToSpec(struct backData *data)
{
    switch(DoMethod(data->pages[data->page],MUIM_Popbackground_GetSpec,&data->spec,&data->grad,MUIV_Popbackground_GetSpec_Image))
    {
        case MUIV_Popbackground_GetSpec_Fail:
            data->flags &= ~FLG_GradientMode;
            break;

        case MUIV_Popbackground_GetSpec_Spec:
            data->flags &= ~FLG_GradientMode;
            break;

        case MUIV_Popbackground_GetSpec_Grad:
            data->flags |= FLG_GradientMode;
            break;
    }
}

/***********************************************************************/

static IPTR
mBackGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct backData *data = INST_DATA(cl,obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Imagedisplay_Spec:
            gadgetsToSpec(data);
            if (data->flags & FLG_GradientMode) return FALSE;
            *msg->opg_Storage = (IPTR)data->spec;
            return TRUE;

        case MUIA_Popbackground_Grad:
            gadgetsToSpec(data);
            if (!(data->flags & FLG_GradientMode)) return FALSE;
            *msg->opg_Storage = (IPTR)&data->grad;
            return TRUE;

        default:
            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static IPTR
mBackSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct backData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for(tstate = msg->ops_AttrList; (tag = NextTagItem((APTR)&tstate)); )
    {
        IPTR tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_Group_ActivePage:
                data->page = tidata;
                break;

            case MUIA_Imagedisplay_Spec:
                specToGadgets(cl,obj,(STRPTR)tidata,NULL);
                break;

            case MUIA_Popbackground_Grad:
                specToGadgets(cl,obj,NULL,(struct MUIS_TheBar_Gradient *)tidata);
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static IPTR
mBackDragQuery(UNUSED struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    STRPTR x;

    if (obj==msg->obj) return MUIV_DragQuery_Refuse;

    if (((get(msg->obj,MUIA_Imagedisplay_Spec,&x) || get(msg->obj,MUIA_Pendisplay_Spec,&x)) && x && *x) ||
        get(msg->obj,MUIA_Popbackground_Grad,&x))
        return MUIV_DragQuery_Accept;

    return MUIV_DragQuery_Refuse;
}

/***********************************************************************/

static IPTR
mBackDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    struct backData *data = INST_DATA(cl,obj);
    IPTR x = 0;

    if ((data->flags & FLG_Gradient) && get(msg->obj,MUIA_Popbackground_Grad,&x))
        set(obj,MUIA_Popbackground_Grad,x);
    else
        if (get(msg->obj,MUIA_Imagedisplay_Spec,&x)) set(obj,MUIA_Imagedisplay_Spec,x);
        else
            if (get(msg->obj,MUIA_Pendisplay_Spec,&x))
            {
                char buf[32];

                snprintf(buf,sizeof(buf),(STRPTR)"2:%s",(char *)x);
                set(obj,MUIA_Imagedisplay_Spec,buf);
            }

    return 0;
}

/***********************************************************************/

DISPATCHER(backDispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:         return mBackNew(cl,obj,(APTR)msg);
        case OM_GET:         return mBackGet(cl,obj,(APTR)msg);
        case OM_SET:         return mBackSets(cl,obj,(APTR)msg);
        case MUIM_DragQuery: return mBackDragQuery(cl,obj,(APTR)msg);
        case MUIM_DragDrop:  return mBackDragDrop(cl,obj,(APTR)msg);
        default:             return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

void
freeBackgroundadjust(void)
{
    freePatterns();
    freeBitmap();
    freeGradient();

    if (lib_backgroundadjust)
    {
        MUI_DeleteCustomClass(lib_backgroundadjust);
        lib_backgroundadjust = NULL;
    }
}

/***********************************************************************/

ULONG
initBackgroundadjust(void)
{
    if (initPatterns() &&
        initBitmap() &&
        initGradient() &&
        (lib_backgroundadjust = MUI_CreateCustomClass(NULL,MUIC_Register,NULL,sizeof(struct backData),ENTRY(backDispatcher))))
    {
        if (lib_flags & BASEFLG_MUI20)
            lib_backgroundadjust->mcc_Class->cl_ID = (STRPTR)"Backgroundadjust";

        return TRUE;
    }

    freeBackgroundadjust();

    return FALSE;
}

/***********************************************************************/
