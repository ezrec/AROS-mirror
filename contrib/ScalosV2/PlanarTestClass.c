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
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>

#include "Scalos.h"
#include "PlanarTestClass.h"
#include "ScalosIntern.h"

#include "SubRoutines.h"
#include "scalos_protos.h"

#include "Debug.h"

/*
static ULONG PlanarTest_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct PlanarTestInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Init\n");

        if (SC_DoSuperMethodA(cl, obj, msg))
        {
                ret = TRUE;
        }

        DEBUG1("Init return value: %ld\n",ret);

        return(ret);
}
*/

static ULONG PlanarTest_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct PlanarTestInst *inst)
{
        DEBUG("Got Setup\n");

        if (SC_DoSuperMethodA(cl, obj, msg))
        {

                inst->bmt_DrInfo    = (struct DrawInfo *)       get(scRenderInfo(obj)->screenobj, SCCA_Screen_DrawInfo);
                inst->bmt_Screen    = (struct Screen *)         get(scRenderInfo(obj)->screenobj, SCCA_Screen_Screen);
                
                /*
                 * We want to get a planar bitmap to test the planar bitmap drawing of the bitmap class
                 */

                // so we get an empty 150x150 planar bitmap ...

                if(!(inst->bmt_PlanarBitmap = AllocBitMap(150,
                                                                                                  150,
                                                                                                  inst->bmt_Screen->BitMap.Depth,
                                                                                                  BMF_DISPLAYABLE|BMF_INTERLEAVED,
                                                                                                  NULL)))
                {
                        // do SCCM_Area_Cleanup on this object and the SCCM_Area_Cleanup`s up the tree (this object to root)
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Area_Cleanup); 
                        
                        DEBUG("Setup return value: 0\n");
                        return FALSE;
                }
                        
                // ... and blit from position (0,0) of the screen to that previously allocated 150x150 planar bitmap
                // so that we have some "nice" graphics data in a planar bitmap structure ...

                BltBitMap(&(inst->bmt_Screen->BitMap),
                                  0,
                                  0,
                                  inst->bmt_PlanarBitmap,
                                  0,
                                  0,
                                  150,
                                  150,
                                  ABNC|ABC,
                                  0xff,       
                                  NULL);

                // ... that we can pass to a BitmapClass object that we make here ...

                if(!(inst->bmt_BitmapObj = SC_NewObject(NULL, SCC_BITMAP_NAME,
                                                                           SCCA_Bitmap_Bitmap, inst->bmt_PlanarBitmap,
                                                                           SCCA_Graphic_Height, 150,
                                                                           SCCA_Graphic_Width, 150,
                                                                           TAG_DONE)))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Area_Cleanup);

                        DEBUG("Setup return value: 0\n");
                        return FALSE;
                }
                
                // ... we can then call SCCM_Graphic_PreThink so that the time consuming stuff that has to be done
                // before we can blit with maximum speed is done
                // and then call SCCM_Graphic_Draw to actually draw the bitmap to the given position in the rastport
                

        }

        if(!(SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PreThinkScreen, (struct Screen *) get(scRenderInfo(obj)->screenobj, SCCA_Screen_Screen))))
        {
                DEBUG("SCCM_Graphic_PreThinkScreen returned failure. Out of memory!\n");
        }
        
        DEBUG("Setup return value: 1\n");
        return(TRUE);
}

static void PlanarTest_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct PlanarTestInst *inst)
{
        DEBUG1("Got Show with action: %ld\n",msg->action);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n", _scleft(obj), _sctop(obj), _scwidth(obj), _scheight(obj));

        if(!(SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PreThinkWindow, scRenderInfo(obj)->rport, msg->action)))
        {
                DEBUG("SCCM_Graphic_PreThinkWindow returns failure. Out of memory.\n");
        }

        DEBUG("Show finished\n");
}

static void PlanarTest_Draw(struct SC_Class *cl, Object *obj, Msg msg, struct PlanarTestInst *inst)
{
        DEBUG("Got Draw\n");

        SC_DoSuperMethodA(cl, obj, (Msg) msg); // SCCM_Area_Draw from AreaClass has no return value

        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n", _scleft(obj), _sctop(obj), _scwidth(obj), _scheight(obj));

        // blit the planar bitmap at position (_scleft(obj),_sctop(obj)) of the window rastport

        SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_Draw, _scleft(obj), _sctop(obj));

        DEBUG("Draw finished\n");
}

// we always do this method for each SCCM_Area_Show
static void PlanarTest_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct PlanarTestInst *inst)
{
        DEBUG1("Got Hide with action: %ld\n",msg->action);

        SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PostThinkWindow, msg->action);
        
        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        DEBUG("Hide finished\n");
}

// we do this method even if our SCCM_Area_Setup returned false, but not if our SCCM_Exit returned false
static void PlanarTest_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct PlanarTestInst *inst)
{
        DEBUG("Got Cleanup\n");
        
        if(inst->bmt_BitmapObj)
        {
                SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PostThinkScreen);
                SC_DisposeObject(inst->bmt_BitmapObj);
        }

        if(inst->bmt_PlanarBitmap) FreeBitMap(inst->bmt_PlanarBitmap);

        SC_DoSuperMethodA(cl, obj, msg);

        DEBUG("Cleanup finished\n");
}

static void PlanarTest_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct PlanarTestInst *inst)
{
        DEBUG("Got AskMinMax\n");

        SC_DoSuperMethodA(cl,obj, (Msg) msg);
        msg->sizes->minwidth += 150;
        msg->sizes->minheight += 150;
        msg->sizes->maxwidth += 150;
        msg->sizes->maxheight += 150;
        msg->sizes->defwidth += 150;
        msg->sizes->defheight += 150;

        DEBUG("AskMinMax finished\n");
}

/*
static void PlanarTest_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct PlanarTestInst *inst)
{
        DEBUG("Got Exit\n");

        SC_DoSuperMethodA(cl, obj, msg);

        DEBUG("Exit finished\n");
}
*/

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData PlanarTestMethods[] =
{
        { SCCM_Area_AskMinMax,(ULONG)   PlanarTest_AskMinMax, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG)       PlanarTest_Setup, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG)        PlanarTest_Show, 0, 0, NULL },
        { SCCM_Area_Draw,(ULONG)        PlanarTest_Draw, 0, 0, NULL },
        { SCCM_Area_Hide,(ULONG)        PlanarTest_Hide, 0, 0, NULL },
        { SCCM_Area_Cleanup,(ULONG)     PlanarTest_Cleanup, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

