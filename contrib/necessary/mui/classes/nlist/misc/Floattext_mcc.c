
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/io.h>
#include <libraries/mui.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/layers.h>
#include <proto/graphics.h>

#include <clib/muimaster_protos.h>

/*
#include <proto/muimaster.h>
#include <inline/muimaster.h>
*/

extern struct TagItem *FindTagItem(Tag,struct TagItem *);


#include "Floattext_priv_mcc.h"
#include "Floattext_mcc.rev.h"

#include "Floattext_mcc.h"

#ifndef SAVEDS
#define SAVEDS
#endif

#ifndef __stdargs
#define __stdargs
#endif


/****************************************************************************************/
/****************************************************************************************/
/******************************                    **************************************/
/******************************  NFloattext Class  **************************************/
/******************************                    **************************************/
/****************************************************************************************/
/****************************************************************************************/

static ULONG mFT_Get(struct IClass *cl,Object *obj,Msg msg)
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



/* ASM ULONG FT_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
ULONG FT_Dispatcher(void)
{
  register struct IClass *a0 __asm("a0");
  struct IClass *cl = a0;
  register Object *a2 __asm("a2");
  Object *obj = a2;
  register Msg a1 __asm("a1");
  Msg msg = a1;

  switch (msg->MethodID)
  {
    case OM_GET : return (mFT_Get(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}

