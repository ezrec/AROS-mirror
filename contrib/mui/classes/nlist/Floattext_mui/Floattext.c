
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/io.h>
#include <libraries/mui.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <clib/alib_protos.h>

#include <proto/muimaster.h>


extern struct TagItem *FindTagItem(Tag,struct TagItem *);


#include "private.h"
#include "rev.h"

#include "Floattext_mui.h"

#ifndef SAVEDS
#define SAVEDS
#endif

#ifndef __stdargs
#define __stdargs
#endif

extern struct Library *MUIMasterBase;

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
      *store = MUIMasterBase->lib_Version;	/* This is nessesary, because MUI checks the version of */
      return(TRUE);							/* opened classes for equalness to muimaster.library. */
    case MUIA_Revision:
      *store = MUIMasterBase->lib_Revision;
      return(TRUE);
  }
  return (DoSuperMethodA(cl,obj,msg));
}



/* ASM ULONG _Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)*/
#ifdef MORPHOS
ULONG _Dispatcher_gate(void)
{
  struct IClass *cl = REG_A0;
  Msg msg = REG_A1;
  Object *obj = REG_A2;
#else
ULONG ASM SAVEDS _Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) )
{
#endif

  switch (msg->MethodID)
  {
    case OM_GET : return (mFT_Get(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}

