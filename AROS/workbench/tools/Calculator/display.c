/*
    Copyright (C) 2012, The AROS Development Team.
    $Id$
*/

#define DEBUG 0
#include <aros/debug.h>

#include <clib/alib_protos.h>

#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <diskfont/diskfont.h>
#include <devices/rawkeycodes.h>
#include <zune/customclasses.h>

#include <stdlib.h>
#include <stdio.h>

#include "display.h"

IPTR CalcDisplay__MUIM_CalcDisplay_DoCurrentStep(struct IClass *cl, Object *obj, Msg msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);

    char outbuf[256];

    double op_A, op_B, op_Res = 0.0;

    if (data->displ_operator == 0)
        return TRUE;

    op_A = atof(data->disp_buff);
    op_B = atof(data->disp_prev);

    if (data->displ_operator == CALCDISPOP_ADD)
    {
        op_Res = op_B + op_A;
    }
    else if (data->displ_operator == CALCDISPOP_SUB)
    {
        op_Res = op_B - op_A;
    }
    else if (data->displ_operator == CALCDISPOP_MUL)
    {
        op_Res = op_B * op_A;
    }
    else if (data->displ_operator == CALCDISPOP_DIV)
    {
        op_Res = op_B / op_A;
    }

    sprintf(outbuf, "%f", op_Res);

    if (data->disp_buff)
        FreeVec(data->disp_buff);

    data->disp_buff = AllocVec(strlen(outbuf) + 1, MEMF_CLEAR);
    CopyMem(outbuf, data->disp_buff, strlen(outbuf));

    return TRUE;
}

IPTR CalcDisplay__MUIM_AskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);
    IPTR height = 3; // spacing

    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth += 100;
    msg->MinMaxInfo->DefWidth += 200;
    msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;

    if (data->disp_font)
        height += data->disp_font->tf_YSize;
    else if ((_rp(obj)) && (_rp(obj)->Font))
        height += _rp(obj)->Font->tf_YSize;
    else
        height += 10;

    if (data->disp_fontsmall)
        height += data->disp_fontsmall->tf_YSize;
    else if ((_rp(obj)) && (_rp(obj)->Font))
        height += _rp(obj)->Font->tf_YSize;
    else
        height += 8;

    msg->MinMaxInfo->MinHeight += height;
    msg->MinMaxInfo->DefHeight += height;
    msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

    return TRUE;
}

IPTR CalcDisplay__OM_NEW(struct IClass *CLASS, Object *obj, struct opSet *message)
{
    struct CalcDisplay_DATA *data = NULL;

    obj = (Object *)DoSuperNewTags(CLASS, obj, NULL,
            ReadListFrame,
        TAG_MORE, (IPTR) message->ops_AttrList);

    if (!obj)
    {
        return FALSE;
    }
   
    data = INST_DATA(CLASS, obj);

    data->disp_textattr.ta_Name = "aroscalculatorregular.font";
    data->disp_textattr.ta_YSize = 22;
    data->disp_textattr.ta_Style = 0;
    data->disp_textattr.ta_Flags = NULL;

    data->disp_smalltextattr.ta_Name = "aroscalculatorregular.font";
    data->disp_smalltextattr.ta_YSize = 8;
    data->disp_smalltextattr.ta_Style = 0;
    data->disp_smalltextattr.ta_Flags = NULL;

    data->disp_font = OpenDiskFont(&data->disp_textattr);
    data->disp_fontsmall = OpenDiskFont(&data->disp_smalltextattr);

    data->disp_buff = NULL;
    data->disp_prev = NULL;

    data->displ_operator = CALCDISPOP_NONE;
    data->displ_flags = CALCDISPFLAG_CALCULATED;

    return (IPTR)obj;
}

IPTR CalcDisplay__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);

    if (data->disp_prev != data->disp_buff)
        FreeVec(data->disp_buff);
    data->disp_buff = NULL;
    FreeVec(data->disp_prev);
    data->disp_prev = NULL;

    return DoSuperMethodA(cl, obj, msg);
}

IPTR CalcDisplay__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);
    APTR clip = NULL;
    char *dispstr = data->disp_buff;
    ULONG dispstrlen, dispstroff, opwidth = 0;

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (dispstr == NULL)
        dispstr ="0";

    D(bug("[Calculator] MUIM_Draw('%s',%d)\n", dispstr, data->displ_operator));

    dispstrlen = strlen(dispstr);

    if (data->disp_font)
        SetFont(_rp(obj), data->disp_font);
    else
        SetFont(_rp(obj), _font(obj));

    dispstroff = TextLength(_rp(obj), dispstr, dispstrlen);

    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
    RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));

    SetAPen(_rp(obj), _pens(obj)[MPEN_TEXT]);

    if (data->displ_operator == CALCDISPOP_ADD)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "+", 1);
        opwidth = TextLength(_rp(obj), "+", 1) + 4;
    }
    else if (data->displ_operator == CALCDISPOP_SUB)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "-", 1);
        opwidth = TextLength(_rp(obj), "-", 1) + 4;
    }
    else if (data->displ_operator == CALCDISPOP_MUL)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "x", 1);
        opwidth = TextLength(_rp(obj), "x", 1) + 4;
    }
    else if (data->displ_operator == CALCDISPOP_DIV)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "/", 1);
        opwidth = TextLength(_rp(obj), "/", 1) + 4;
    }

    clip = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj) + opwidth, (_mbottom(obj) - _rp(obj)->Font->tf_YSize), (_mright(obj) - (_mleft(obj) + opwidth) + 1), _rp(obj)->Font->tf_YSize);
    Move(_rp(obj), (_mright(obj) - dispstroff) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
    Text(_rp(obj), dispstr, dispstrlen);
    MUI_RemoveClipping(muiRenderInfo(obj), clip);

    return TRUE;
}

IPTR CalcDisplay__OM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;
    IPTR  rv    = TRUE;

    switch (msg->opg_AttrID)
    {
        case MUIA_CalcDisplay_Input:
            *store = (IPTR)data->disp_buff;
            break;

        default:
            rv = DoSuperMethodA(cl, obj, (Msg)msg);
    }
    
    return rv;
}

IPTR CalcDisplay__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);
    struct TagItem      *tags = msg->ops_AttrList;
    struct TagItem      *tag;

    while ((tag = NextTagItem((const struct TagItem **)&tags)) != NULL)
    {
	switch (tag->ti_Tag)
	{
            case MUIA_CalcDisplay_Calculated:
                if (tag->ti_Data)
                {
                    data->displ_flags |= CALCDISPFLAG_CALCULATED;
                }
                else
                {
                    data->displ_flags &= ~CALCDISPFLAG_CALCULATED;
                }
                break;

	    case MUIA_CalcDisplay_Input:
                if ((tag->ti_Data >= (IPTR)'0') && (tag->ti_Data <= (IPTR)'9'))
                {
                    if ((data->disp_buff == NULL) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                    {
                        if (data->displ_flags & CALCDISPFLAG_CLEAROP)
                        {
                            data->displ_flags &= ~CALCDISPFLAG_CLEAROP;
                            data->displ_operator = CALCDISPOP_NONE;
                        }
                        data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                        if (data->disp_buff != data->disp_prev)
                            FreeVec(data->disp_buff);
                        data->disp_buff = AllocVec(2, MEMF_CLEAR);
                        data->disp_buff[0]= (UBYTE)tag->ti_Data;
                    }
                    else
                    {
                        char *oldbuff = data->disp_buff;
                        ULONG oldlen = strlen(oldbuff);
                        if (oldlen < MUIV_CalcDisplay_MaxInputLen)
                        {
                            data->disp_buff = AllocVec(oldlen + 2, MEMF_CLEAR);
                            CopyMem(oldbuff, data->disp_buff, oldlen);
                            data->disp_buff[oldlen] = (UBYTE)tag->ti_Data;
                            FreeVec(oldbuff);
                        }
                    }
                    SET(obj, MUIA_CalcDisplay_Calculated, FALSE);
                }
                if (((tag->ti_Data >= (IPTR)'A') && (tag->ti_Data <= (IPTR)'F')) || ((tag->ti_Data >= (IPTR)'a') && (tag->ti_Data <= (IPTR)'f')))
                {
                    if ((data->disp_buff == NULL) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                    {
                        if (data->displ_flags & CALCDISPFLAG_CLEAROP)
                        {
                            data->displ_flags &= ~CALCDISPFLAG_CLEAROP;
                            data->displ_operator = CALCDISPOP_NONE;
                        }
                        data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                        if (data->disp_buff != data->disp_prev)
                            FreeVec(data->disp_buff);
                        data->disp_buff = AllocVec(2, MEMF_CLEAR);
                        data->disp_buff[0]= (UBYTE)tag->ti_Data;
                    }
                    else
                    {
                        char *oldbuff = data->disp_buff;
                        ULONG oldlen = strlen(oldbuff);
                        if (oldlen < MUIV_CalcDisplay_MaxInputLen)
                        {
                            data->disp_buff = AllocVec(oldlen + 2, MEMF_CLEAR);
                            CopyMem(oldbuff, data->disp_buff, oldlen);
                            data->disp_buff[oldlen] = (UBYTE)tag->ti_Data;
                            FreeVec(oldbuff);
                        }
                    }
                    SET(obj, MUIA_CalcDisplay_Calculated, FALSE);
                }
                else if (tag->ti_Data == (IPTR)'.')
                {
                    if (!(data->displ_flags & CALCDISPFLAG_HASPERIOD))
                    {
                        if ((data->disp_buff == NULL) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                        {
                            if (data->displ_flags & CALCDISPFLAG_CLEAROP)
                            {
                                data->displ_flags &= ~CALCDISPFLAG_CLEAROP;
                                data->displ_operator = CALCDISPOP_NONE;
                            }

                            if (data->disp_buff != data->disp_prev)
                                FreeVec(data->disp_buff);

                            data->disp_buff = AllocVec(3, MEMF_CLEAR);
                            data->disp_buff[0]= '0';
                            data->disp_buff[1]= (UBYTE)tag->ti_Data;
                        }
                        else
                        {
                            char *oldbuff = data->disp_buff;
                            ULONG oldlen = strlen(oldbuff);
                            if (oldlen < MUIV_CalcDisplay_MaxInputLen)
                            {
                                data->disp_buff = AllocVec(oldlen + 2, MEMF_CLEAR);
                                CopyMem(oldbuff, data->disp_buff, oldlen);
                                data->disp_buff[oldlen] = (UBYTE)tag->ti_Data;
                                FreeVec(oldbuff);
                            }
                        }
                        SET(obj, MUIA_CalcDisplay_Calculated, FALSE);
                        data->displ_flags |= CALCDISPFLAG_HASPERIOD;
                    }
                }
                else if (tag->ti_Data == (IPTR)'+')
                {
                    if (data->disp_prev)
                    {
                        DoMethod(obj, MUIM_CalcDisplay_DoCurrentStep);
                    }
                    data->displ_operator = CALCDISPOP_ADD;
                    data->disp_prev = data->disp_buff;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'-')
                {
                    if (data->disp_prev)
                    {
                        DoMethod(obj, MUIM_CalcDisplay_DoCurrentStep);
                    }
                    data->displ_operator = CALCDISPOP_SUB;
                    data->disp_prev = data->disp_buff;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'*')
                {
                    if (data->disp_prev)
                    {
                        DoMethod(obj, MUIM_CalcDisplay_DoCurrentStep);
                    }
                    data->displ_operator = CALCDISPOP_MUL;
                    data->disp_prev = data->disp_buff;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'/')
                {
                    if (data->disp_prev)
                    {
                        DoMethod(obj, MUIM_CalcDisplay_DoCurrentStep);
                    }
                    data->displ_operator = CALCDISPOP_DIV;
                    data->disp_prev = data->disp_buff;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)-1)
                {
                    // CA
                    data->displ_operator = CALCDISPOP_NONE;
                    if (data->disp_prev != data->disp_buff)
                        FreeVec(data->disp_buff);
                    data->disp_buff = NULL;
                    FreeVec(data->disp_prev);
                    data->disp_prev = NULL;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)-2)
                {
                    // CE
                    if (data->disp_prev != data->disp_buff)
                        FreeVec(data->disp_buff);

                    data->disp_buff = NULL;

                    if (!(data->disp_prev))
                    {
                        SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                    }
                }
                else if (tag->ti_Data == (IPTR)'=')
                {
                    if (data->disp_prev)
                    {
                        DoMethod(obj, MUIM_CalcDisplay_DoCurrentStep);
                    }
                    data->displ_flags |= CALCDISPFLAG_CLEAROP;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
		MUI_Redraw(obj, MADF_DRAWOBJECT);
		break;
	}
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

BOOPSI_DISPATCHER(IPTR, CalcDisplay_Dispatcher, CLASS, self, message)
{
    switch (message->MethodID)
    {
    case MUIM_Draw:
        return CalcDisplay__MUIM_Draw(CLASS, self, (struct MUIP_Draw *)message);

    case OM_SET:
        return CalcDisplay__OM_SET(CLASS, self, (struct opSet *)message);

    case MUIM_CalcDisplay_DoCurrentStep:
        return CalcDisplay__MUIM_CalcDisplay_DoCurrentStep(CLASS, self, message);

    case OM_GET:
        return CalcDisplay__OM_GET(CLASS, self, (struct opGet *)message);

    case MUIM_AskMinMax:
        return CalcDisplay__MUIM_AskMinMax(CLASS, self, (struct MUIP_AskMinMax *)message);

    case OM_NEW:
        return CalcDisplay__OM_NEW(CLASS, self, (struct opSet *)message);

    case OM_DISPOSE:
        return CalcDisplay__OM_DISPOSE(CLASS, self, Msg message);

    default:
        return DoSuperMethodA(CLASS, self, message);
    }

    return 0;
}
BOOPSI_DISPATCHER_END
