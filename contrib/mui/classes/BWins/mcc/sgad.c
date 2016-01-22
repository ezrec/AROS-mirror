
#include "class.h"

/***********************************************************************/

static IPTR ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct opSet *msg)
{
    if ((obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
    {
        struct Gadget *gad = (struct Gadget *)obj;

        gad->Flags      |= GFLG_RELSPECIAL;
        gad->GadgetType |= GetTagData(MUIA_BWin_Type,MUIDEF_BWin_Type,msg->ops_AttrList);
    }

    return (IPTR)obj;
}

/***************************************************************************/

#ifdef __AROS__
static BOOPSI_DISPATCHER(IPTR,dispatcher,cl,obj,msg)
#else
static ULONG ASM SAVEDS
dispatcher(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg )
#endif
{
    switch (msg->MethodID)
    {
        case OM_NEW: return mNew(cl,obj,(APTR)msg);
        default:     return DoSuperMethodA(cl,obj,msg);
    }
}
#ifdef __AROS__
    BOOPSI_DISPATCHER_END
#endif

/*******************************************************************/

BOOL ASM
initSGad(void)
{
    if ((lib_sgad = MakeClass(NULL,"gadgetclass",NULL,0,0)))
    {
        lib_sgad->cl_Dispatcher.h_SubEntry = NULL;
        lib_sgad->cl_Dispatcher.h_Entry    = (HOOKFUNC)dispatcher;
        lib_sgad->cl_Dispatcher.h_Data     = NULL;

        return TRUE;
    }

    return FALSE;
}

/*******************************************************************/
