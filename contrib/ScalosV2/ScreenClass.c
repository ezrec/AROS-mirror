// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <proto/guigfx.h>

#include "Scalos.h"
#include "ScreenClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"

#include "scalos_protos.h"

/*----------------------------------------------------------------------*/

extern struct MinList ScreenList;
extern struct SignalSemaphore ScreenListSem;

/*--------------------------- Functions --------------------------------*/

static ULONG Screen_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct ScreenInst *inst)
{
        char *name;

        if (SC_DoSuperMethodA(cl,obj,(Msg )msg))
        {
                if ((name = (char *) GetTagData(SCCA_Screen_PubName,0,msg->ops_AttrList)))
                {
                        if (!(inst->screen = LockPubScreen(name)))
                        {
                                SC_DoSuperMethod(cl,obj,OM_DISPOSE);
                                return(FALSE);
                        }
                
                }
                if (inst->screen)
                {
                        struct ScreenNode *node;
                        struct ScreenNode *foundnode = 0;

                        inst->drawinfo = GetScreenDrawInfo(inst->screen);
                        ObtainSemaphore(&ScreenListSem);
                        for (node = (struct ScreenNode *) ScreenList.mlh_Head; node->node.mln_Succ; node = (struct ScreenNode *) node->node.mln_Succ)
                        {
                                if (node->screen == inst->screen)
                                {
                                        foundnode = node;
                                        break;
                                }
                        }
                        if (!(foundnode))
                                if ((foundnode = AllocNode(&ScreenList, sizeof(struct ScreenNode))))
                                {
                                        foundnode->screen = inst->screen;
                                        foundnode->pensharemap = CreatePenShareMapA(NULL);
                                }

                        if (foundnode)
                        {
                                foundnode->count += 1;
                                inst->screennode = foundnode;
                        }
                        ReleaseSemaphore(&ScreenListSem);
                }
                return(TRUE);
        }
        return(FALSE);
}


static void Screen_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct ScreenInst *inst )
{
        if (inst->screennode)
        {
                ObtainSemaphore(&ScreenListSem);
                if (!(inst->screennode->count -= 1))
                {
                        DeletePenShareMap(inst->screennode->pensharemap);
                        FreeNode(inst->screennode);
                }
                ReleaseSemaphore(&ScreenListSem);
        }

        if (inst->screen)
        {
                FreeScreenDrawInfo(inst->screen,inst->drawinfo);
                UnlockPubScreen(NULL,inst->screen);
        }
        SC_DoSuperMethodA(cl,obj,msg);
}

static void Screen_Set( struct SC_Class *cl, Object *obj, struct opSet *msg, struct ScreenInst *inst )
{
        SC_DoSuperMethodA( cl,obj,(Msg )msg );
//      if (inst->window)
//              SetTags( msg->ops_AttrList,inst );
}

static ULONG Screen_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct ScreenInst *inst )
{
        if (msg->opg_AttrID == SCCA_Screen_Screen)
        {
                *(msg->opg_Storage) = (ULONG) inst->screen;
                return(TRUE);
        }
        if (msg->opg_AttrID == SCCA_Screen_DrawInfo)
        {
                *(msg->opg_Storage) = (ULONG) inst->drawinfo;
                return(TRUE);
        }
        if (msg->opg_AttrID == SCCA_Screen_PenShareMap)
        {
                if (inst->screennode)
                        *(msg->opg_Storage) = (ULONG) inst->screennode->pensharemap;
                else
                        *(msg->opg_Storage) = 0;
                return(TRUE);
        }
        return(FALSE);
}

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData ScreenMethods[] =
{
        { SCCM_Init,(ULONG) Screen_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) Screen_Exit, 0, 0, NULL },
        { OM_SET,(ULONG) Screen_Set, 0, 0, NULL },
        { OM_GET,(ULONG) Screen_Get, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

