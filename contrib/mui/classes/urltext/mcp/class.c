
#include "class.h"
#include <libraries/asl.h>

#include <stdio.h>

/****************************************************************************/

#define ALFIEHP         "http://web.tiscalinet.it/amiga"
#define ALFIEEMAIL      "alforan@tin.it"
#define ALFIEEMAILURL   "mailto:" ALFIEEMAIL

/****************************************************************************/

static IPTR ASM
mNew(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    char           copyright[256], *l;
    struct data    *data;
    Object         *info, *popb;
    STRPTR                 ver;

    if (!(obj = (Object *)DoSuperMethodA(cl, obj, msg)))
        return 0;

    data = INST_DATA(cl,obj);
    memset(data,0,sizeof(struct data));

    data->prefs = VGroup,

        Child, HGroup,

            Child, HGroup,
                GroupFrameT(getString(Msg_Colours)),
                Child, HSpace(0),
                Child, VGroup,
                    GroupSpacing(0),
                    Child, VSpace(0),
                    Child, ColGroup(2),
                        Child, Label2(l = getString(Msg_Out)),
                        Child, data->mouseOutPen = keyPopPen(Msg_OutPenTitle,l,Msg_Out_Help),
                        Child, Label2(l = getString(Msg_Over)),
                        Child, data->mouseOverPen = keyPopPen(Msg_OverPenTitle,l,Msg_Over_Help),
                        Child, Label2(l = getString(Msg_Visited)),
                        Child, data->visitedPen = keyPopPen(Msg_VisitedPenTitle,l,Msg_Visited_Help),
                    End,
                    Child, VSpace(0),
                End,
                Child, HSpace(0),
            End,

            Child, VGroup,
                GroupFrameT(getString(Msg_Settings)),
                Child, VSpace(0),
                Child, HGroup,
                    Child, HSpace(0),
                    Child,ColGroup(2),
                        Child, Label1(l = getString(Msg_Underline)),
                        Child, data->underline = keyCheckmark(l,Msg_Underline_Help),
                        Child, Label1(l = getString(Msg_Fallback)),
                        Child, data->fallBack = keyCheckmark(l,Msg_Fallback_Help),
                        Child, Label1(l = getString(Msg_DoVisitedPen)),
                        Child, data->doVisitedPen = keyCheckmark(l,Msg_DoVisitedPen_Help),
                    End,
                    Child, HSpace(0),
                End,
                Child, VSpace(0),
                Child, HGroup,
                    Child, Label2(l = getString(Msg_Font)),
                    Child, PopaslObject,
                        MUIA_ShortHelp, getString(Msg_Font_Help),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        MUIA_Popstring_String, data->font = StringObject,
                            StringFrame,
                            MUIA_CycleChain, TRUE,
                            MUIA_ControlChar, getKeyChar(l),
                        End,
                        MUIA_Popstring_Button, popb = MUI_MakeObject(MUIO_PopButton,MUII_PopUp),
                    End,
                End,
            End,
        End,

        Child, VGroup,
            GroupFrameT(getString(Msg_Examples)),
            MUIA_Background, MUII_GroupBack,
            Child, VSpace(0),
            Child, HGroup,
                Child, HSpace(0),
                Child, data->url = urltext(ALFIEHP,NULL),
                Child, HSpace(0),
                Child, data->email = urltext(ALFIEEMAILURL,ALFIEEMAIL),
                Child, HSpace(0),
            End,
            Child, VSpace(0),
        End,

        Child, info = TextObject,
            TextFrame,
            MUIA_Background, MUII_TextBack,
            MUIA_FixHeightTxt, "\n",
        End,
    End;

    if (!data->prefs)
    {
        CoerceMethod(cl,obj,OM_DISPOSE);
        return 0;
    }

    get(data->url,MUIA_Urltext_Version,&ver);
    snprintf(copyright,sizeof(copyright),"\33b\33c%s\33n\n%s",ver,getString(Msg_Copyright));
    set(info,MUIA_Text_Contents,copyright);

    set(popb,MUIA_CycleChain,TRUE);

    DoMethod(obj,OM_ADDMEMBER,data->prefs);

    DoMethod(data->mouseOutPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_MouseOutPen,MUIV_TriggerValue);
    DoMethod(data->mouseOutPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_MouseOutPen,MUIV_TriggerValue);

    DoMethod(data->mouseOverPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_MouseOverPen,MUIV_TriggerValue);
    DoMethod(data->mouseOverPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_MouseOverPen,MUIV_TriggerValue);

    DoMethod(data->visitedPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_VisitedPen,MUIV_TriggerValue);
    DoMethod(data->visitedPen,MUIM_Notify,MUIA_Pendisplay_Spec,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_VisitedPen,MUIV_TriggerValue);

    DoMethod(data->underline,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_PUnderline,MUIV_TriggerValue);
    DoMethod(data->underline,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_PUnderline,MUIV_TriggerValue);

    DoMethod(data->fallBack,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_PFallBack,MUIV_TriggerValue);
    DoMethod(data->fallBack,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_PFallBack,MUIV_TriggerValue);

    DoMethod(data->doVisitedPen,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->url,3,MUIM_Set,MUIA_Urltext_PDoVisitedPen,MUIV_TriggerValue);
    DoMethod(data->doVisitedPen,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
        data->email,3,MUIM_Set,MUIA_Urltext_PDoVisitedPen,MUIV_TriggerValue);

    return (IPTR)obj;
}

/***********************************************************************/

static IPTR
mGet(struct IClass *cl, Object *obj, struct opGet *msg)
{
    switch (msg->opg_AttrID)
    {
        case MUIA_Version:  *msg->opg_Storage = VERSION; return TRUE;
        case MUIA_Revision: *msg->opg_Storage = REVISION; return TRUE;
        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

static IPTR ASM
mDispose(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static IPTR ASM
mSetup(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Setup *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    IPTR                   p;

    if (!DoSuperMethodA(cl,obj,(APTR)msg)) return FALSE;

    get(data->mouseOutPen,MUIA_Pendisplay_Spec,&p);
    set(data->mouseOutPen,MUIA_Pendisplay_Spec,p);

    get(data->mouseOverPen,MUIA_Pendisplay_Spec,&p);
    set(data->mouseOverPen,MUIA_Pendisplay_Spec,p);

    get(data->visitedPen,MUIA_Pendisplay_Spec,&p);
    set(data->visitedPen,MUIA_Pendisplay_Spec,p);

    return TRUE;
}

/***********************************************************************/

static IPTR ASM
mConfigToGadgets(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Settingsgroup_ConfigToGadgets *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct MUI_PenSpec      *pen;
    Object         *po;
    ULONG                   p;

    po = MUI_NewObject(MUIC_Pendisplay,TAG_DONE);

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_MouseOutPen)))
        set(data->mouseOutPen,MUIA_Pendisplay_Spec,p);
    else
        if (po)
        {
            DoMethod(po,MUIM_Pendisplay_SetMUIPen,DEFAULT_MOUSEOUT_PEN);
            get(po,MUIA_Pendisplay_Spec,&pen);
            set(data->mouseOutPen,MUIA_Pendisplay_Spec,pen);
        }

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_MouseOverPen)))
        set(data->mouseOverPen,MUIA_Pendisplay_Spec,p);
    else
        if (po)
        {
            DoMethod(po,MUIM_Pendisplay_SetMUIPen,DEFAULT_MOUSEOVER_PEN);
            get(po,MUIA_Pendisplay_Spec,&pen);
            set(data->mouseOverPen,MUIA_Pendisplay_Spec,pen);
        }

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_VisitedPen)))
        set(data->visitedPen,MUIA_Pendisplay_Spec,p);
    else
        if (po)
        {
            DoMethod(po,MUIM_Pendisplay_SetMUIPen,DEFAULT_VISITED_PEN);
            get(po,MUIA_Pendisplay_Spec,&pen);
            set(data->visitedPen,MUIA_Pendisplay_Spec,pen);
        }

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_Underline)))
        set(data->underline,MUIA_Selected,*(ULONG *)p);
    else set(data->underline,MUIA_Selected,DEFAULT_UNDERLINE);

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_FallBack)))
        set(data->fallBack,MUIA_Selected,*(ULONG *)p);
    else set(data->fallBack,MUIA_Selected,DEFAULT_FALLBACK);

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_DoVisitedPen)))
        set(data->doVisitedPen,MUIA_Selected,*(ULONG *)p);
    set(data->doVisitedPen,MUIA_Selected,DEFAULT_DOVISITEDPEN);

    if ((p = DoMethod(msg->configdata,MUIM_Dataspace_Find,MUIA_Urltext_Font)))
        set(data->font,MUIA_String_Contents,p);

    if (po) MUI_DisposeObject(po);

    return 0;
}

/***********************************************************************/

static IPTR ASM
mGadgetsToConfig(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) struct MUIP_Settingsgroup_GadgetsToConfig *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    IPTR                    p;

    get(data->mouseOutPen,MUIA_Pendisplay_Spec,&p);
    if (p) DoMethod(msg->configdata,MUIM_Dataspace_Add,p,sizeof(struct MUI_PenSpec),MUIA_Urltext_MouseOutPen);

    get(data->mouseOverPen,MUIA_Pendisplay_Spec,&p);
    if (p) DoMethod(msg->configdata,MUIM_Dataspace_Add,p,sizeof(struct MUI_PenSpec),MUIA_Urltext_MouseOverPen);

    get(data->visitedPen,MUIA_Pendisplay_Spec,&p);
    if (p) DoMethod(msg->configdata,MUIM_Dataspace_Add,p,sizeof(struct MUI_PenSpec),MUIA_Urltext_VisitedPen);

    get(data->underline,MUIA_Selected,&p);
    DoMethod(msg->configdata,MUIM_Dataspace_Add,&p,4,MUIA_Urltext_Underline);

    get(data->fallBack,MUIA_Selected,&p);
    DoMethod(msg->configdata,MUIM_Dataspace_Add,&p,4,MUIA_Urltext_FallBack);

    get(data->doVisitedPen,MUIA_Selected,&p);
    DoMethod(msg->configdata,MUIM_Dataspace_Add,&p,4,MUIA_Urltext_DoVisitedPen);

    get(data->font,MUIA_String_Contents,&p);
    if (*((STRPTR)p)) DoMethod(msg->configdata,MUIM_Dataspace_Add,p,strlen((STRPTR)p)+1,MUIA_Urltext_Font);
    else DoMethod(msg->configdata,MUIM_Dataspace_Remove,MUIA_Urltext_Font);
    return 0;
}

/***********************************************************************/

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR,dispatcher,cl,obj,msg)
#else
static SAVEDS ASM ULONG
dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
#endif
{
    switch(msg->MethodID)
    {
        case OM_NEW:                                return mNew(cl,obj,(APTR)msg);
        case OM_GET:                                return mGet(cl,obj,(APTR)msg);
        case OM_DISPOSE:                            return mDispose(cl,obj,(APTR)msg);
        case MUIM_Setup:                            return mSetup(cl,obj,(APTR)msg);
        case MUIM_Settingsgroup_ConfigToGadgets:    return mConfigToGadgets(cl,obj,(APTR)msg);
        case MUIM_Settingsgroup_GadgetsToConfig:    return mGadgetsToConfig(cl,obj,(APTR)msg);
        default:                                    return DoSuperMethodA(cl,obj,msg);
    }
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

/***********************************************************************/

BOOL ASM
initMCP(REG(a0) struct UrltextBase *base)
{
    if ((base->mcp = MUI_CreateCustomClass((struct Library *)base,MUIC_Mccprefs,NULL,sizeof(struct data),dispatcher)))
    {
        if (MUIMasterBase->lib_Version>=20)
            base->mcp->mcc_Class->cl_ID = PRG;

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/
