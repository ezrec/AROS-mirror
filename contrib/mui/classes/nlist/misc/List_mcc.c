
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libraries/mui.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <clib/muimaster_protos.h>

#include "List_priv_mcc.h"
#include "List_mcc.rev.h"
#include "Listview_mcc.h"

#ifndef MUI_NList_MCC_H
#include <MUI/NList_mcc.h>
#endif

#ifndef SAVEDS
#define SAVEDS
#endif

#ifndef __stdargs
#define __stdargs
#endif

/****************************************************************************************/
/****************************************************************************************/
/******************************                    **************************************/
/******************************     NList Class    **************************************/
/******************************                    **************************************/
/****************************************************************************************/
/****************************************************************************************/

static ULONG mL_Get(struct IClass *cl,Object *obj,Msg msg)
{
  LONG  *store = ((struct opGet *)msg)->opg_Storage;

  switch (((struct opGet *)msg)->opg_AttrID)
  {
    case MUIA_Version:
      *store = LIB_VERSION;
      return(TRUE);
    case MUIA_Revision:
      *store = LIB_REVISION;
      return(TRUE);
  }
  return (DoSuperMethodA(cl,obj,msg));
}


/* ASM ULONG L_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
ULONG L_Dispatcher(LONG pad)
{
  register struct IClass *a0 __asm("a0");
  struct IClass *cl = a0;
  register Object *a2 __asm("a2");
  Object *obj = a2;
  register Msg a1 __asm("a1");
  Msg msg = a1;

  switch (msg->MethodID)
  {
    case OM_GET : return (mL_Get(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}

