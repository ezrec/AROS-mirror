
#include "private.h"

#ifdef _AROS
#include <proto/intuition.h>
#endif

struct MUI_CustomClass *NLI_Class = NULL;



static ULONG mNLI_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  if (data->DoDraw)
    DoSuperMethodA(cl,obj,(Msg) msg);

  return(0);
}


static ULONG mNLI_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NLIData *data;

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg) msg)))
    return(0);

  data = INST_DATA(cl,obj);
  data->DoDraw = FALSE;

  return((ULONG) obj);
}


static ULONG mNLI_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for (tags=((struct opSet *)msg)->ops_AttrList;tag=(struct TagItem *) NextTagItem(&tags);)
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Image_Spec:
        tag->ti_Tag = TAG_IGNORE;
        data->DoDraw = TRUE;
        MUI_Redraw(obj,MADF_DRAWOBJECT);
        data->DoDraw = FALSE;
        break;
      case MUIA_FillArea:
      case MUIA_Font:
      case MUIA_Background:
        tag->ti_Tag = TAG_IGNORE;
        break;
    }
  }
  return (DoSuperMethodA(cl,obj,msg));
}


#ifdef MORPHOS
ULONG NLI_Dispatcher_gate(void)
{
  struct IClass *cl = REG_A0;
  Msg msg = REG_A1;
  Object *obj = REG_A2;
#elif defined(_AROS)
AROS_UFH3(ULONG, NLI_Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
  AROS_USERFUNC_INIT
#else
ULONG ASM SAVEDS NLI_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) )
{
#endif

  switch (msg->MethodID)
  {
    case OM_NEW           : return (  mNLI_New(cl,obj,(APTR)msg));
    case MUIM_HandleInput : return (0);
    case OM_SET           : return (  mNLI_Set(cl,obj,(APTR)msg));
    case MUIM_Draw        : return ( mNLI_Draw(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
#ifdef _AROS
  AROS_USERFUNC_EXIT
#endif
}


#ifdef MORPHOS
MADTRAP(NLI_Dispatcher);
#endif

struct MUI_CustomClass *NLI_Create(void)
{
  NLI_Class = MUI_CreateCustomClass(NULL,MUIC_Image,NULL,sizeof(struct NLIData),&NLI_Dispatcher);
  return (NLI_Class);
}


void NLI_Delete(void)
{
  if (NLI_Class)
    MUI_DeleteCustomClass(NLI_Class);
  NLI_Class = NULL;
}

