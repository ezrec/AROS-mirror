
#include "private.h"


#define MUIA_NList_Visible                  0x9d510063 /* GM  ..g  LONG              */

struct MUI_CustomClass *NGR_Class = NULL;


static ULONG mNGR_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NGRData *data;
  struct TagItem *tag;

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg) msg)))
    return(0);

  data = INST_DATA(cl,obj);
  data->DoDraw = FALSE;

  return((ULONG) obj);
}

/*
 * static ULONG mNGR_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
 * {
 *   register struct NGRData *data = INST_DATA(cl,obj);
 *
 * D(bug("%lx|group|draw0 %lx %lx \n",obj,msg->flags,muiAreaData(obj)->mad_Flags));
 *
 *   if (data->DoDraw)
 *     DoSuperMethodA(cl,obj,(Msg) msg);
 *
 * D(bug("%lx|group|draw1 %lx %lx \n",obj,msg->flags,muiAreaData(obj)->mad_Flags));
 *
 *   return(0);
 * }
 */


static ULONG mNGR_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NGRData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for (tags=((struct opSet *)msg)->ops_AttrList;tag=(struct TagItem *) NextTagItem(&tags);)
  {
    switch (tag->ti_Tag)
    { case MUIA_NList_Visible :
        data->DoDraw = tag->ti_Data;
    }
  }
  return (0);
}


static ULONG mNGR_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
  register struct NLData *data = INST_DATA(cl,obj);
/*D(bug("%lx|grp_AskMinMax() 1 \n",obj));*/
  DoSuperMethodA(cl,obj,(Msg) msg);
/*D(bug("%lx|grp_AskMinMax() 2 \n",obj));*/
  msg->MinMaxInfo->MinWidth  = MUI_MAXMAX+100;
  msg->MinMaxInfo->DefWidth  = MUI_MAXMAX+100;
  msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX+100;
  msg->MinMaxInfo->MinHeight = MUI_MAXMAX+100;
  msg->MinMaxInfo->DefHeight = MUI_MAXMAX+100;
  msg->MinMaxInfo->MaxHeight = MUI_MAXMAX+100;
/*
 *   msg->MinMaxInfo->MinWidth  += MUI_MAXMAX;
 *   msg->MinMaxInfo->DefWidth  += MUI_MAXMAX;
 *   msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;
 *   msg->MinMaxInfo->MinHeight += MUI_MAXMAX;
 *   msg->MinMaxInfo->DefHeight += MUI_MAXMAX;
 *   msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;
 * {
 * LONG w = (LONG) msg->MinMaxInfo->DefWidth;
 * LONG h = (LONG) msg->MinMaxInfo->DefHeight;
 * LONG w1 = (LONG) msg->MinMaxInfo->MinWidth;
 * LONG h1 = (LONG) msg->MinMaxInfo->MinHeight;
 * D(bug("%lx|group|w=%ld h=%ld (%ld,%ld) \n",obj,w,h,w1,h1));
 * }
 */
  return(0);
}


#ifdef MORPHOS
ULONG NGR_Dispatcher_gate(void)
{
  struct IClass *cl = REG_A0;
  Msg msg = REG_A1;
  Object *obj = REG_A2;
#else
ULONG ASM SAVEDS NGR_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) )
{
#endif

  switch (msg->MethodID)
  {
    case OM_NEW           : return (mNGR_New(cl,obj,(APTR)msg));
    case MUIM_AskMinMax   : return (mNGR_AskMinMax(cl,obj,(APTR)msg));
    case OM_SET           : return (0);
    case MUIM_Draw        : return (0);
    case MUIM_HandleInput : return (0);
    case MUIM_NoNotifySet : return (0);
  }
  return(DoSuperMethodA(cl,obj,msg));
}

#ifdef MORPHOS
MADTRAP(NGR_Dispatcher);
#endif


struct MUI_CustomClass *NGR_Create(void)
{
  NGR_Class = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct NGRData),&NGR_Dispatcher);
  return (NGR_Class);
}


void NGR_Delete(void)
{
  if (NGR_Class)
    MUI_DeleteCustomClass(NGR_Class);
  NGR_Class = NULL;
}

