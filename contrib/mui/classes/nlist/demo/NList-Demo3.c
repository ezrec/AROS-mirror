
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>

#ifdef __GNUC__
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <clib/muimaster_protos.h>
#endif

#ifdef __SASC
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
extern struct Library *SysBase,*IntuitionBase,*UtilityBase,*GfxBase,*DOSBase,*IconBase;
#endif

extern struct Library *MUIMasterBase;

#ifdef __AROS__
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <aros/asmcall.h>
#include <MUI/NList_mcc.h>
#endif

#include <MUI/NListview_mcc.h>
#include <MUI/NFloattext_mcc.h>

#include "NList-Demo3.h"


#ifdef __SASC
#include <proto/muimaster.h>
#endif


/* *********************************************** */


struct MUI_CustomClass *NLI_Class = NULL;


/* *********************************************** */


struct NLIData
{
  LONG special;
  LONG EntryCurrent;
  LONG EntryHeight;
};


/* *********************************************** */


ULONG mNLI_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  DoSuperMethodA(cl,obj,(Msg) msg);
  if ((msg->flags & MADF_DRAWOBJECT) || (msg->flags & MADF_DRAWUPDATE))
  { WORD x1,x2,x3,x4,x5,y1,y2,y3,y4,y5;
    y1 = _top(obj);
    y2 = _bottom(obj);
    x1 = _left(obj);
    x2 = _right(obj);
    if ((data->special == 0) || (data->special == 1))
    {
      y3 = (y1+y2)/2;
      x3 = (x1+x2)/2;
      SetAPen(_rp(obj),_pens(obj)[MPEN_MARK]);
      SetBPen(_rp(obj),_pens(obj)[MPEN_SHADOW]);
      SetDrMd(_rp(obj),JAM2);
      SetDrPt(_rp(obj),(UWORD) ~0);
      if      (data->special == 0)
      { Move(_rp(obj), x3-2, y1+1);
        Draw(_rp(obj), x3-2, y2-1);
        Move(_rp(obj), x3, y1+1);
        Draw(_rp(obj), x3, y2-1);
        Move(_rp(obj), x3+2, y1+1);
        Draw(_rp(obj), x3+2, y2-1);
      }
      else if (data->special == 1)
      { Move(_rp(obj), x1, y3-2);
        Draw(_rp(obj), x2, y3-2);
        Move(_rp(obj), x1, y3);
        Draw(_rp(obj), x2, y3);
        Move(_rp(obj), x1, y3+2);
        Draw(_rp(obj), x2, y3+2);
      }
      SetAPen(_rp(obj),_pens(obj)[MPEN_SHADOW]);
      Move(_rp(obj), x1, y2-1);
      Draw(_rp(obj), x1, y1+1);
      Draw(_rp(obj), x2, y1+1);
      SetAPen(_rp(obj),_pens(obj)[MPEN_SHINE]);
      Draw(_rp(obj), x2, y2-1);
      Draw(_rp(obj), x1, y2-1);
      SetDrMd(_rp(obj),JAM1);
    }
    else if (((x2 - x1) >= 10) && ((y2 - y1) >= 8))   /* and special==2 to 9 */
    {
      y3 = (y1+y2)/2;
      x3 = x1 + 1;
      x2--;
      SetAPen(_rp(obj),_pens(obj)[MPEN_SHADOW]);
      SetDrMd(_rp(obj),JAM1);

      y4 = y1;
      x4 = x3 + 2;
      y5 = y2;
      x5 = x2-6;
      if ((data->EntryHeight & 1) && (data->EntryCurrent & 1))
        y4++;
      if ((y4 & 1) != (y3 & 1))
        x4--;
      if (data->special > 5)
        x5 = x2;
      if (data->special & 1)
        y5 = y3;
      while (y4 <= y5)
      { WritePixel(_rp(obj), x3, y4);
        y4 += 2;
      }
      if (data->special <= 7)
      {
        while (x4 <= x5)
        { WritePixel(_rp(obj), x4, y3);
          x4 += 2;
        }
      }
      if (data->special <= 5)
      {
        Move(_rp(obj), x2-6, y3);
        Draw(_rp(obj), x2-6, y3-3);
        Draw(_rp(obj),   x2, y3-3);
        Draw(_rp(obj),   x2, y3+3);
        Draw(_rp(obj), x2-6, y3+3);
        Draw(_rp(obj), x2-6, y3);
        Move(_rp(obj), x2-4, y3);
        Draw(_rp(obj), x2-2, y3);
        if ((data->special == 2) || (data->special == 3))
        { Move(_rp(obj), x2-3, y3-1);
          Draw(_rp(obj), x2-3, y3+1);
        }
      }
    }
  }
  msg->flags = 0;
  return(0);
}


ULONG mNLI_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NLIData *data;
  if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg) msg)))
    return(0);
  data = INST_DATA(cl,obj);
  data->special = 0;
  return((ULONG) obj);
}


ULONG mNLI_AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
  DoSuperMethodA(cl,obj,(Msg) msg);
  msg->MinMaxInfo->MinWidth  += 8;
  msg->MinMaxInfo->DefWidth  += 18; /* the only width def value really used by NList object */
  msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;
  msg->MinMaxInfo->MinHeight += 7;  /* the only height def value really used by NList object */
  msg->MinMaxInfo->DefHeight += 12;
  msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;
  return(0);
}


ULONG mNLI_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;
  for (tags=((struct opSet *)msg)->ops_AttrList;tag=(struct TagItem *) NextTagItem(&tags);)
  { switch (tag->ti_Tag)
    {
      case MUIA_NLIMG_EntryCurrent:
        data->EntryCurrent = tag->ti_Data;
        break;
      case MUIA_NLIMG_EntryHeight:
        data->EntryHeight = tag->ti_Data;
        break;
      case MUIA_NLIMG_Spec:
        data->special = tag->ti_Data;
        break;
    }
  }
  return (0);
}


#ifdef __SASC
SAVEDS ASM ULONG NLI_Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
#endif
#ifdef __GNUC__
#ifdef __AROS__
AROS_UFH3(ULONG, NLI_Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
  AROS_USERFUNC_INIT
#else
ULONG NLI_Dispatcher(void)
{ register struct IClass *a0 __asm("a0"); struct IClass *cl = a0;
  register Object *a2 __asm("a2");        Object *obj = a2;
  register Msg a1 __asm("a1");            Msg msg = a1;
#endif
#endif

  switch (msg->MethodID)
  {
    case OM_NEW         : return (      mNLI_New(cl,obj,(APTR)msg));
    case OM_SET         : return (      mNLI_Set(cl,obj,(APTR)msg));
    case MUIM_AskMinMax : return (mNLI_AskMinMax(cl,obj,(APTR)msg));
    case MUIM_Draw      : return (     mNLI_Draw(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));

#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}




struct MUI_CustomClass *NLI_Create(void)
{
  NLI_Class = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct NLIData),NLI_Dispatcher);
  return (NLI_Class);
}


void NLI_Delete(void)
{
  if (NLI_Class)
    MUI_DeleteCustomClass(NLI_Class);
  NLI_Class = NULL;
}


