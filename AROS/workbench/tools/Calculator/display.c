/*
    Copyright (C) 2012, The AROS Development Team.
    $Id$
*/

#define DEBUG 1
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

#include "display.h"

IPTR CalcDisplay__MUIM_AskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth += 200;
    msg->MinMaxInfo->DefWidth += 200;
    msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;

    msg->MinMaxInfo->MinHeight += 30;
    msg->MinMaxInfo->DefHeight += 30;
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

    data->disp_font = OpenDiskFont(&data->disp_textattr);
    
    data->disp_buff = NULL;
    data->disp_prev = NULL;

    data->displ_operator = CALCDISPOP_NONE;
    data->displ_flags = CALCDISPFLAG_CALCULATED;

    return (IPTR)obj;
}

IPTR CalcDisplay__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);

    FreeVec(data->disp_buff);
    data->disp_buff = NULL;
    FreeVec(data->disp_prev);
    data->disp_prev = NULL;

    return DoSuperMethodA(cl, obj, msg);
}

IPTR CalcDisplay__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct CalcDisplay_DATA *data = INST_DATA(cl, obj);

    char *dispstr = data->disp_buff;
    ULONG dispstrlen, dispstroff;

bug("[Calculator] MUIM_Draw()\n");

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (dispstr == NULL)
        dispstr ="0";

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
    }
    else if (data->displ_operator == CALCDISPOP_SUB)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "-", 1);
    }
    else if (data->displ_operator == CALCDISPOP_MUL)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "x", 1);
    }
    else if (data->displ_operator == CALCDISPOP_DIV)
    {
        Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
        Text(_rp(obj), "/", 1);
    }
    
    Move(_rp(obj), (_mright(obj) - dispstroff) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
    Text(_rp(obj), dispstr, dispstrlen);

    return TRUE;
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
            data->displ_flags |= CALCDISPFLAG_CALCULATED;
	    case MUIA_CalcDisplay_Input:
                if ((tag->ti_Data >= (IPTR)'0') && (tag->ti_Data <= (IPTR)'9'))
                {
                    if ((!data->disp_buff) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                    {
                        FreeVec(data->disp_buff);
                        data->disp_buff = AllocVec(2, MEMF_CLEAR);
                        data->disp_buff[0]= (UBYTE)tag->ti_Data;
                    }
                    else
                    {
                        char *oldbuff = data->disp_buff;
                        ULONG oldlen = strlen(oldbuff);
                        if (oldlen < 32)
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
                    if ((!data->disp_buff) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                    {
                        FreeVec(data->disp_buff);
                        data->disp_buff = AllocVec(2, MEMF_CLEAR);
                        data->disp_buff[0]= (UBYTE)tag->ti_Data;
                    }
                    else
                    {
                        char *oldbuff = data->disp_buff;
                        ULONG oldlen = strlen(oldbuff);
                        if (oldlen < 32)
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
                        if ((!data->disp_buff) || (data->displ_flags & CALCDISPFLAG_CALCULATED))
                        {
                            FreeVec(data->disp_buff);
                            data->disp_buff = AllocVec(3, MEMF_CLEAR);
                            data->disp_buff[0]= '0';
                            data->disp_buff[1]= (UBYTE)tag->ti_Data;
                        }
                        else
                        {
                            char *oldbuff = data->disp_buff;
                            ULONG oldlen = strlen(oldbuff);
                            if (oldlen < 32)
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
                    }
                    data->displ_operator = CALCDISPOP_ADD;
                    data->disp_prev = data->disp_buff;
                    data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'-')
                {
                    if (data->disp_prev)
                    {
                    }
                    data->displ_operator = CALCDISPOP_SUB;
                    data->disp_prev = data->disp_buff;
                    data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'*')
                {
                    if (data->disp_prev)
                    {
                    }
                    data->displ_operator = CALCDISPOP_MUL;
                    data->disp_prev = data->disp_buff;
                    data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)'/')
                {
                    if (data->disp_prev)
                    {
                    }
                    data->displ_operator = CALCDISPOP_DIV;
                    data->disp_prev = data->disp_buff;
                    data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)-1)
                {
                    // CA
                    data->displ_operator = CALCDISPOP_NONE;
                    FreeVec(data->disp_buff);
                    data->disp_buff = NULL;
                    FreeVec(data->disp_prev);
                    data->disp_prev = NULL;
                    SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                }
                else if (tag->ti_Data == (IPTR)-2)
                {
                    // CE
                    FreeVec(data->disp_buff);
                    data->disp_buff = NULL;
                    if (!(data->disp_prev))
                    {
                        SET(obj, MUIA_CalcDisplay_Calculated, TRUE);
                    }
                }
                else if (tag->ti_Data == (IPTR)'=')
                {
                    data->displ_operator = CALCDISPOP_NONE;
                    if (data->disp_prev)
                    {

                    }
                    data->displ_flags &= ~CALCDISPFLAG_HASPERIOD;
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

    case MUIM_AskMinMax:
        return CalcDisplay__MUIM_AskMinMax(CLASS, self, (struct MUIP_AskMinMax *)message);

    case OM_NEW:
        return CalcDisplay__OM_NEW(CLASS, self, (struct opSet *)message);

    case OM_DISPOSE:
        return CalcDisplay__OM_DISPOSE(CLASS, self, (struct opSet *)message);

    default:
        return DoSuperMethodA(CLASS, self, message);
    }

    return 0;
}
BOOPSI_DISPATCHER_END
