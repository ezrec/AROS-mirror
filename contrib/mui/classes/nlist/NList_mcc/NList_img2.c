
#include "private.h"

#ifdef _AROS
#include <proto/intuition.h>
#endif

struct MUI_CustomClass *NLI2_Class = NULL;



static ULONG mNLI2_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  if (data->DoDraw)
    DoSuperMethodA(cl,obj,(Msg) msg);

  return(0);
}


static ULONG mNLI2_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NLIData *data;

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg) msg)))
    return(0);

  data = INST_DATA(cl,obj);
  data->DoDraw = FALSE;

  return((ULONG) obj);
}


static ULONG mNLI2_Set(struct IClass *cl,Object *obj,Msg msg)
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
ULONG NLI2_Dispatcher_gate(void)
{
  struct IClass *cl = REG_A0;
  Msg msg = REG_A1;
  Object *obj = REG_A2;
#elif defined(_AROS)
AROS_UFH3(ULONG, NLI2_Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
  AROS_USERFUNC_INIT
#else
ULONG ASM SAVEDS NLI2_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) )
{
#endif

  switch (msg->MethodID)
  {
    case OM_NEW        : return (  mNLI2_New(cl,obj,(APTR)msg));
    case OM_SET        : return (  mNLI2_Set(cl,obj,(APTR)msg));
    case MUIM_Draw     : return ( mNLI2_Draw(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));

#ifdef _AROS
  AROS_USERFUNC_EXIT
#endif
}


#ifdef MORPHOS
MADTRAP(NLI2_Dispatcher);
#endif



struct MUI_CustomClass *NLI2_Create(void)
{
  NLI2_Class = MUI_CreateCustomClass(NULL,MUIC_Image,NULL,sizeof(struct NLIData),&NLI_Dispatcher);
  return (NLI2_Class);
}


void NLI2_Delete(void)
{
  if (NLI2_Class)
    MUI_DeleteCustomClass(NLI2_Class);
  NLI2_Class = NULL;
}

