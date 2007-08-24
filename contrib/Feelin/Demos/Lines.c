/*
   
   XMLLines Demo © 2000-2005 by Olivier LAVIALE <gofromiel@gofromiel.com>

   This example illustrate how to writte a  custom  class  using  a  methods
   table instead of a dispatcher. This class doesn't use Dynamic IDs.

*/

///Header

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/xmlapplication.h>

struct FeelinBase                  *FeelinBase;
#define GfxBase                     FeelinBase->Graphics

enum    {  // methods

        FM_Lines_Update

        };

enum    {  // attributes

        FA_Lines_Cycle,
        FA_Lines_Trail,
        FA_Lines_Micro

        };

struct LocalObjectData
{
    FAreaPublic                     *AreaPublic;
    FSignalHandler                   SignalHandler;

    int16                             x[2],y[2];
    int16                             xd[2],yd[2];
    APTR                             ox[2],oy[2];
    uint32                            j;
    uint32                            Pen;        // FV_Pen_Xxx
    uint8                            Cycle;
    uint8                            CycleDone;
    int8                             CycleWay;
    uint8                            Line;       // Line to draw (or beeing drawn)
    int8                             k;
    uint8                            Trail;
    APTR                             TrailData;
};

long rand(void);

#define MyLine                                  F_NewObj(cc -> Name,TAG_DONE)

//+

///ModifyTrail
APTR ModifyTrail(FClass *Class, FObject Obj, uint8 Trail)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> Trail = 0;

    if (LOD -> TrailData)
    {
        LOD -> ox[0] = 0; LOD -> ox[1] = 0; // Reset coordinates
        LOD -> oy[0] = 0; LOD -> oy[1] = 0; // Reset coordinates

        F_Dispose(LOD -> TrailData);
    }
        
    LOD -> TrailData = F_New(sizeof (int16) * 4 * Trail);
    
    if (LOD->TrailData)
    {
       LOD -> ox[0] = LOD -> TrailData;
       LOD -> ox[1] = (APTR)((uint32)(LOD -> ox[0]) + sizeof (int16) * Trail);
       LOD -> oy[0] = (APTR)((uint32)(LOD -> ox[1]) + sizeof (int16) * Trail);
       LOD -> oy[1] = (APTR)((uint32)(LOD -> oy[0]) + sizeof (int16) * Trail);
    }

    LOD -> Trail = Trail;

    return LOD -> TrailData;
}
//+

/// mNew
F_METHOD(FObject,mNew)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    LOD -> Cycle                     = 1;
    LOD -> Trail                     = 8;
    LOD -> SignalHandler.Object      = Obj;
    LOD -> SignalHandler.Method      = F_IDM(FM_Lines_Update);
    LOD -> SignalHandler.Flags       = FF_SignalHandler_Timer;
    LOD -> SignalHandler.fsh_Secs    = 0;
    LOD -> SignalHandler.fsh_Micros  = 10000;

    if (F_SuperDo(Class,Obj,Method,

        FA_ChainToCycle,        FALSE,
        FA_Back,                FI_Dark,
        FA_Frame,              "$gauge-frame",

        TAG_MORE, Msg))
    {
        if (!LOD -> TrailData)
        {
            ModifyTrail(Class,Obj,LOD -> Trail);
        }

        if (LOD -> TrailData)
        {
            return Obj;
        }
    }
    return NULL;
}
//+
///mDispose
F_METHOD(void,mDispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> TrailData)
    {
        F_Dispose(LOD -> TrailData); LOD -> TrailData = NULL;
    }

    F_SUPERDO();
}
//+
///mSet
F_METHOD(void,mSet)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Lines_Cycle:
        {
            LOD -> Cycle = item.ti_Data;
            LOD -> Pen = (LOD -> Cycle) ? FV_Pen_Shine : FV_Pen_Highlight;
        }
        break;

        case FA_Lines_Trail:
        {
           if (FF_Area_CanDraw & _flags)
           {
               F_Set(_render,FA_Render_Forbid,TRUE);

               ModifyTrail(Class,Obj,item.ti_Data);

               F_Set(_render,FA_Render_Forbid,FALSE);

               F_Draw(Obj,FF_Draw_Object);
           }
           else
           {
               ModifyTrail(Class,Obj,item.ti_Data);
           }
        }
        break;

        case FA_Lines_Micro:
        {
            LOD -> SignalHandler.fsh_Micros = item.ti_Data;
        }
        break;
    }

    F_SUPERDO();
}
//+
/// mShow
F_METHOD(int32,mShow)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

/* It's better to add signal handlers in the  FM_Show  method  because  the
object  may  be hidden at any time and even if it won't be able to be drawn
it's a waste of time (CPU time) to leave it active. */

    if (F_SUPERDO())
    {
        F_Do(_app,FM_Application_AddSignalHandler,&LOD -> SignalHandler);

        return TRUE;
    }
    return FALSE;
}
//+
/// mHide
F_METHOD(void,mHide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (FF_SignalHandler_Active & LOD -> SignalHandler.Flags)
    {
        F_Do(_app,FM_Application_RemSignalHandler,&LOD -> SignalHandler);
    }
    F_SUPERDO();
}
//+
/// mAskMinMax

/* AskMinMax method will be called before the window is opened  and  before
layout  takes place. We need to tell Feelin the minimum and maximum size of
our object. Note that we indeed need to *add* these values,  not  just  set
them ! */

F_METHOD(uint32,mAskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _minw += 30;
    _minh += 30;

    /* Now call our superclass. FC_Area will handle everything, taking  care
    of FA_FixedXxx, FA_MinXxx and FA_MaxXxx. */

    return F_SUPERDO();
}
//+
/// mDraw

/* Draw method is called whenever Feelin feels (obviously  ;-))  we  should
render our object. This usually happens after layout is finished. Note: You
may only render within the rectangle _ix, _iy, _ix2, _iy2. */

F_METHODM(void,mDraw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;

    int16 *ox;
    int16 *oy;
    
    uint32 i;

    /* let our superclass draw itself first, Area class would e.g. draw  the
    frame and clear the whole region. What it does exactly depends on flags.
    */

    if (FF_Draw_Object & Msg -> Flags)
    {
        F_SUPERDO();
        
        /* Reset values */

        for (i = 0 ; i < 2 ; i++)
        {
            LOD ->  x[i] = _ix + (rand() % _iw); if (LOD -> x[i] < _ix) LOD -> x[i] = _ix; else if (LOD -> x[i] > _ix2) LOD -> x[i] = _ix2;
            LOD ->  y[i] = _iy + (rand() % _ih); if (LOD -> y[i] < _iy) LOD -> y[i] = _iy; else if (LOD -> y[i] > _iy2) LOD -> y[i] = _iy2;
            LOD -> xd[i] = rand() % 10 + 1;
            LOD -> yd[i] = rand() % 10 + 1;
        }

        LOD -> Pen        = (LOD -> Cycle) ? FV_Pen_Shine : FV_Pen_Highlight;
        LOD -> j          = 0;
        LOD -> CycleWay   = 1;
        LOD -> CycleDone  = 0;
        LOD -> Line       = 0;
    }

    if (!LOD -> TrailData) return;

    if (LOD -> j >= LOD -> Trail - 1)
    {
        i = (LOD -> Line >= LOD -> Trail - 1) ? 0 : LOD -> Line + 1;

        _FPen(FV_Pen_Dark);
        ox = LOD -> ox[0]; oy = LOD -> oy[0]; _Move(ox[i],oy[i]);
        ox = LOD -> ox[1]; oy = LOD -> oy[1]; _Draw(ox[i],oy[i]);
    }

    _FPen(LOD -> Pen);
    _Move(LOD -> x[0],LOD -> y[0]);
    _Draw(LOD -> x[1],LOD -> y[1]);

    if (LOD -> Cycle && ++LOD -> CycleDone >= LOD -> Cycle)
    {
        LOD -> CycleDone = 0;

        if (LOD -> CycleWay > 0)
        {
            if (LOD -> Pen + 1 >= FV_Pen_Dark) LOD -> CycleWay = -1;
        }
        else
        {
            if (LOD -> Pen - 1 <= FV_Pen_Shine) LOD -> CycleWay = 1;
        }

        LOD -> Pen += LOD -> CycleWay;
    }

    for (i = 0 ; i < 2 ; i++)
    {
        ox = LOD -> ox[i]; ox[LOD -> Line] = LOD -> x[i];
        oy = LOD -> oy[i]; oy[LOD -> Line] = LOD -> y[i];

        LOD -> x[i] += LOD -> xd[i];
        LOD -> y[i] += LOD -> yd[i];

        if (LOD -> x[i] < _ix)
        {
            LOD -> xd[i] = -LOD -> xd[i]; LOD -> x[i] = _ix;
        }
        else if (LOD -> x[i] > _ix2)
        {
            LOD -> xd[i] = -LOD -> xd[i]; LOD -> x[i] = _ix2;
        }

        if (LOD -> y[i] < _iy)
        {
            LOD -> yd[i] = -LOD -> yd[i]; LOD -> y[i] = _iy;
        }
        else if (LOD -> y[i] > _iy2)
        {
            LOD -> yd[i] = -LOD -> yd[i]; LOD -> y[i] = _iy2;
        }

        /* Twisting coordinates */

        if (((rand() >> 5) & 127) < 2)
        {
            if (LOD -> xd[i] < 1) LOD -> k = TRUE;
            LOD -> xd[i] = (rand() >> 5) & 7;
            if (LOD -> k) LOD -> xd[i] = -LOD -> xd[i];
            LOD -> k = FALSE;
        }

        if (((rand() >> 5) & 255) < 50)
        {
             if (LOD -> yd[i] < 1) LOD -> k = TRUE;
             LOD -> yd[i] = (rand() >> 5) & 7;
             if (LOD -> k) LOD -> yd[i] = -LOD -> yd[i];
             LOD -> k = FALSE;
        }
    }

    if (++LOD -> Line >= LOD -> Trail) LOD -> Line = 0;
    LOD -> j++;
}
//+
///mUpdate
F_METHOD(int32,mUpdate)
{
    F_Draw(Obj, FF_Draw_Update);
   
    return TRUE; // If we return FALSE the timer event won't be requested again
}
//+

/*** attributes and methods defined by the class ***************************/

STATIC F_ATTRIBUTES_ARRAY =
{
    F_ATTRIBUTES_ADD("Cycle", FV_TYPE_INTEGER),
    F_ATTRIBUTES_ADD("Trail", FV_TYPE_INTEGER),
    F_ATTRIBUTES_ADD("Micro", FV_TYPE_INTEGER),

    F_ARRAY_END
};

STATIC F_METHODS_ARRAY =
{
    F_METHODS_ADD(mUpdate, "Update"),

    F_METHODS_ADD_STATIC(mNew,       FM_New),
    F_METHODS_ADD_STATIC(mDispose,   FM_Dispose),
    F_METHODS_ADD_STATIC(mSet,       FM_Set),
    F_METHODS_ADD_STATIC(mShow,      FM_Show),
    F_METHODS_ADD_STATIC(mHide,      FM_Hide),
    F_METHODS_ADD_STATIC(mAskMinMax, FM_AskMinMax),
    F_METHODS_ADD_STATIC(mDraw,      FM_Draw),

    F_ARRAY_END
};

STATIC F_TAGS_ARRAY =
{
    F_TAGS_ADD_SUPER(Area),
    F_TAGS_ADD_LOD,
    F_TAGS_ADD_METHODS,
    F_TAGS_ADD_ATTRIBUTES,
    
    F_ARRAY_END
};

/*** main ******************************************************************/

///Main
int32 main()
{
    if (F_FEELIN_OPEN)
    {
        /* Create the new custom class with a call to F_CreateClassA().  This
        function  returns  a  struct  FeelinClass. You must use cc -> Name to
        create instance of your custom class. This Name is unique and made by
        F_CreateClassA(). */

        FClass *cc = F_CreateClassA(NULL,F_TAGS);
        
        if (cc)
        {
            FObject xapp, app,win,grp,ccl,trl,spd;
            
            F_XML_REFS_INIT(6);
            F_XML_DEFS_INIT(1);
             
            F_XML_REFS_ADD("app",&app);
            F_XML_REFS_ADD("win",&win);
            F_XML_REFS_ADD("grp",&grp);
            F_XML_REFS_ADD("ccl",&ccl);
            F_XML_REFS_ADD("trl",&trl);
            F_XML_REFS_ADD("spd",&spd);
            F_XML_REFS_DONE;
            
            F_XML_DEFS_ADD("class:line",cc);
            F_XML_DEFS_DONE;

            xapp = XMLApplicationObject,
                
                "Source",        "lines.xml",
                "SourceType",    "File",
                "Definitions",   F_XML_DEFS,
                "References",    F_XML_REFS,

                End;
                             
            if (xapp)
            {
                /* create notifications */
     
                F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
                F_Do(ccl,FM_Notify,"Value",FV_Notify_Always,grp,FM_Set,4,F_ID(cc -> Attributes,FA_Lines_Cycle),FV_Notify_Value,FA_Group_Forward,TRUE);
                F_Do(trl,FM_Notify,"Value",FV_Notify_Always,grp,FM_Set,4,F_ID(cc -> Attributes,FA_Lines_Trail),FV_Notify_Value,FA_Group_Forward,TRUE);
                F_Do(spd,FM_Notify,"Value",FV_Notify_Always,grp,FM_Set,4,F_ID(cc -> Attributes,FA_Lines_Micro),FV_Notify_Value,FA_Group_Forward,TRUE);
                
                /* launch the application */
     
                F_Do(xapp,(uint32) "Run");

                F_DisposeObj(xapp);
            }

            F_DeleteClass(cc);
        }
        else
        {
            Printf("Unable to create custom class.\n");
        }
        F_FEELIN_CLOSE;
    }
    else
    {
        Printf("Unable to open feelin.library v%ld.\n",FV_FEELIN_VERSION);
    }
    return 0;
}
//+
