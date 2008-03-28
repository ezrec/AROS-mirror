/********************************************************************

Spinner gadget for the interrupt button, by Michael Fischer 1994.

This will initially have a line radiating from the center of the
gadget at 0, 45, 90, ... degree angles.

Eventually it will use images, perhaps of a rotating Boing ball.

*********************************************************************/

/* We really don't need _all_ these... try to narrow it down some time */

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "gui.h"
#include "XtoI.h"
#include "protos.h"
#include "spinner.h"

struct SpinnerClData {
  int state;
};

/*
** AskMinMax method will be called before the window is opened
** and before layout takes place. We need to tell MUI the
** minimum, maximum and default size of our object.
*/

static ULONG SpinnerClAskMinMax(Class *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
  /*
   ** let our superclass first fill in what it thinks about sizes.
   ** this will e.g. add the size of frame and inner spacing.
   */
  
  DoSuperMethodA(cl,obj,(Msg)msg);
  
  /*
   ** now add the values specific to our object. note that we
   ** indeed need to *add* these values, not just set them!
   */
  
  msg->MinMaxInfo->MinWidth  += 10;
  msg->MinMaxInfo->DefWidth  += 11;
  msg->MinMaxInfo->MaxWidth  += 15;

  msg->MinMaxInfo->MinHeight += 10;
  msg->MinMaxInfo->DefHeight += 15;
  msg->MinMaxInfo->MaxHeight += 30;

  return(0);
}



static ULONG SpinnerClRender(Class *cl, Object *obj, struct MUIP_Draw *msg)
{
  struct SpinnerClData *inst = INST_DATA(cl, obj);
  int midx, midy, x, y;

  /*
   ** let our superclass draw itself first, area class would
   ** e.g. draw the frame and clear the whole region. What
   ** it does exactly depends on msg->flags.
   */
  
  DoSuperMethodA(cl,obj,(Msg)msg);

  midx = _mright(obj) - _mwidth(obj)/2;
  midy = _mbottom(obj) - _mheight(obj)/2;
  
  switch (inst->state) {
  case 0:
    x = midx; y = _mtop(obj); break;
  case 1:
    x = _mright(obj); y = _mtop(obj); break;
  case 2:
    x = _mright(obj); y = midy; break;
  case 3:
    x = _mright(obj); y = _mbottom(obj); break;
  case 4:
    x = midx; y = _mbottom(obj); break;
  case 5:
    x = _mleft(obj); y = _mbottom(obj); break;
  case 6:
    x = _mleft(obj); y = midy; break;
  case 7:
    x = _mleft(obj); y = _mtop(obj); break;
  }

  if (msg->flags & MADF_DRAWOBJECT) {
    SetAPen(_rp(obj), _dri(obj)->dri_Pens[BACKGROUNDPEN]);
    RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));
    SetAPen(_rp(obj), _dri(obj)->dri_Pens[TEXTPEN]);
    Move(_rp(obj), midx, midy);
    Draw(_rp(obj), x, y);
  }

  else if (msg->flags & MADF_DRAWUPDATE) { /* called from our input method */
    SetAPen(_rp(obj), _dri(obj)->dri_Pens[BACKGROUNDPEN]);
    RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));
    SetAPen(_rp(obj), _dri(obj)->dri_Pens[TEXTPEN]);
    Move(_rp(obj), midx, midy);
    Draw(_rp(obj), x, y);
  }

  return 0;
}


static ULONG SpinnerClSetup(Class *cl, Object *obj, Msg msg)
{
  if (!(DoSuperMethodA(cl, obj, (Msg)msg)))
    return (FALSE);

  MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS);

  return (TRUE);
}


static ULONG SpinnerClCleanup(Class *cl, Object *obj, Msg msg)
{
  MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS);
  return (DoSuperMethodA(cl, obj, (Msg)msg));
}


#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#undef _isinobject
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))


static ULONG SpinnerClHandleInput(Class *cl, Object *obj, 
				  struct MUIP_HandleInput *msg)
{
  if (!(DoSuperMethodA(cl, obj, (Msg)msg)))
    return (FALSE);

  return(0);
}


static ULONG SpinnerClSet(Class *cl, Object *obj, Msg msg)
{
  struct SpinnerClData *inst = INST_DATA(cl, obj);

  (inst->state)++;
  if (inst->state > 7) inst->state = 0;
  MUI_Redraw(obj, MADF_DRAWOBJECT);

  return (DoSuperMethodA(cl, obj, msg));
}


/********************************************************
  The main gadget handler routine
********************************************************/
#ifndef __AROS__
ASM ULONG SpinnerClDispatch(REG(a0) Class *cl, REG(a2) Object *obj,
			    REG(a1) Msg msg)
{
#else
AROS_UFH3(ULONG,SpinnerClDispatch,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
  AROS_USERFUNC_INIT
#endif
  switch (msg->MethodID)
    {
    case MUIM_AskMinMax: return(SpinnerClAskMinMax(cl,obj,(APTR)msg));
    case MUIM_Draw     : return(SpinnerClRender(cl,obj,(APTR)msg));
    case MUIM_HandleInput: return(SpinnerClHandleInput(cl,obj,(APTR)msg));
    case MUIM_Setup    : return(SpinnerClSetup(cl,obj,(APTR)msg));
    case MUIM_Cleanup  : return(SpinnerClCleanup(cl,obj,(APTR)msg));
    case OM_SET        : return(SpinnerClSet(cl,obj,(APTR)msg));
    }
  
  return(DoSuperMethodA(cl,obj,msg));
#ifdef __AROS__
  AROS_USERFUNC_EXIT
#endif
}


/************************************************************
  Initilize the Spinner class
************************************************************/
Class *SpinnerClInit(void)
{
  APTR SuperClass;
  Class *cl = NULL;

  /* Get a pointer to the MUI superclass. */

  if (!(SuperClass = MUI_GetClass(MUIC_Area)))
    fail(NULL, "Superclass for spinner class not found.");

  /* Create the MUI spinner class */

  if (!(cl = MakeClass(NULL, NULL, SuperClass, sizeof(struct SpinnerClData), 0)))
    {
      MUI_FreeClass(SuperClass);
      fail(NULL, "Failed to create spinner class.");
    }

  /* Set the class dispatcher */

  cl->cl_Dispatcher.h_Entry = (APTR)SpinnerClDispatch;
  cl->cl_Dispatcher.h_SubEntry = NULL;
  cl->cl_Dispatcher.h_Data = NULL;

  return cl;
}


/*******************************************************
  Free the Spinner class
  IS THERE AN ERROR HERE BECAUSE SUPERCLASS IS NEVER FREED?
*******************************************************/
BOOL SpinnerClFree(Class *cl)
{
  FreeClass(cl);
  return 0;
}


