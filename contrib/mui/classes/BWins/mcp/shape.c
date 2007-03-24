
#include "class.h"

/***********************************************************************/

struct data
{
    char  buf[16];
    ULONG type;
    ULONG flags;
};

/***********************************************************************/

static ULONG STDARGS
DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
    return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}

/***********************************************************************/

static ULONG ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    register struct TagItem *attrs = msg->ops_AttrList;

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_CycleChain,  TRUE,
            MUIA_Numeric_Min, 0,
            MUIA_Numeric_Max, 3,
            TAG_MORE, attrs))
    {
        register struct data *data = INST_DATA(cl,obj);

        data->type = GetTagData(MUIA_BWin_Type,MUIDEF_BWin_Type,attrs);
    }

    return (ULONG)obj;
}

/***************************************************************************/

static SAVEDS ULONG ASM
mStringify(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Numeric_Stringify *msg)
{
    register struct data *data = INST_DATA(cl,obj);

    switch(msg->value)
    {
        case MUIV_BWin_Shape_None:
            stccpy(data->buf,getString(MCPMSG_Shape_None),sizeof(data->buf));
            break;

        case MUIV_BWin_Shape_Original:
            stccpy(data->buf,getString(MCPMSG_Shape_Original),sizeof(data->buf));
            break;

        case MUIV_BWin_Shape_Line:
            stccpy(data->buf,getString(MCPMSG_Shape_Line),sizeof(data->buf));
            break;

        case MUIV_BWin_Shape_Solid:
            stccpy(data->buf,getString(MCPMSG_Shape_Solid),sizeof(data->buf));
            break;
    }

    return (ULONG)data->buf;
}

/***************************************************************************/

static ULONG ASM SAVEDS
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    switch(msg->MethodID)
    {
        case OM_NEW:                 return mNew(cl,obj,(APTR)msg);
        case MUIM_Numeric_Stringify: return mStringify(cl,obj,(APTR)msg);
        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

/***************************************************************************/

BOOL ASM
initShape(void)
{
    if (lib_shape = MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,sizeof(struct data),dispatcher))
    {
        if (MUIMasterBase->lib_Version>=20)
            lib_shape->mcc_Class->cl_ID = "BWinShape";

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
