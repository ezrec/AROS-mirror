/* checkbox gadget subclass */

#include <proto/bgui.h>
#include <proto/intuition.h>
#include <libraries/bgui_macros.h>
#include "checkbox.h"

#ifdef __AROS__
extern struct Library *BGUIBase;
#endif

Class *CheckClass;

ULONG Check_NEW(Class *cl, Object *obj, struct opSet *ops)
{
  ULONG rc = 0;
  Object *check;

  check = BGUI_NewObject(BGUI_VECTOR_IMAGE,VIT_BuiltIn,BUILTIN_CHECKMARK,
    VIT_DriPen,TEXTPEN,TAG_DONE);
  if (check)
  {
    struct TagItem tags[6];
    tags[0].ti_Tag = BUTTON_SelectedVector;
    tags[0].ti_Data = (ULONG)check;
    tags[1].ti_Tag = LAB_NoPlaceIn;
    tags[1].ti_Data = TRUE;
    tags[2].ti_Tag = BT_NoRecessed;
    tags[2].ti_Data = TRUE;
    tags[3].ti_Tag = FRM_EdgesOnly;
    tags[3].ti_Data = FALSE;
    tags[4].ti_Tag = GA_ToggleSelect;
    tags[4].ti_Data = TRUE;
    tags[5].ti_Tag = TAG_MORE;
    tags[5].ti_Data = (ULONG)ops->ops_AttrList;

    rc = DoSuperMethod(cl,obj,OM_NEW,tags,NULL);
    if (rc == 0)
      DoMethod(check,OM_DISPOSE);
  }
  return rc;
}

#ifdef __AROS__
AROS_UFH3(ULONG, DispatchCheckGadg,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
#else
SAVEDS ASM ULONG DispatchCheckGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, obj), TF_REGPARAM(a1, Msg, msg))
#endif
{
    AROS_USERFUNC_INIT

  ULONG retval;

  switch (msg->MethodID)
  {
  case OM_NEW:
    retval = Check_NEW(cl,obj,(struct opSet *)msg);
    break;
  default:
    retval = DoSuperMethodA(cl,obj,msg);
    break;
  }
  return retval;

    AROS_USERFUNC_EXIT
}

Class *InitCheckClass(void)
{
  if (CheckClass == NULL)
  {
    Class *super;

    if ((super = BGUI_GetClassPtr(BGUI_BUTTON_GADGET)))
    {
      CheckClass = MakeClass(NULL,NULL,super,0,0);
      if (CheckClass)
	CheckClass->cl_Dispatcher.h_Entry = (HOOKFUNC)DispatchCheckGadg;
    }
  }
  return CheckClass;
}

void FreeCheckClass(void)
{
  if (CheckClass)
    FreeClass(CheckClass);
  CheckClass = NULL;
}
