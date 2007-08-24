#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Window_Setup

/* This method is called by the method FM_Window_Open to setup  the  object
according  to  the  user  preferences  and  the  future environement of the
window. */

F_METHOD(BOOL,Window_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint32 data;

    /* The most important thing done within this method, is the creation  of
    the  FC_Render  object.  This object is shared between objects among the
    window. This object contains everything you need to know about the whole
    application. No need to say that such an object is very important. */

    LOD -> SYSFlags &= ~FF_WINDOW_SYS_KNOWNMINMAX;

    LOD -> Render = RenderObject,

        FA_Render_Application, LOD -> Application,
        FA_Render_Window,      Obj,
        
        End;
                                             
    if (LOD -> Render == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create FC_Render object");

        return FALSE;
    }

    LOD -> Screen = (struct Screen *) F_Get(LOD -> Render -> Display, (uint32) "FA_Display_Screen");
    LOD -> Scheme = (FPalette *) F_Do(LOD -> Render -> Display,FM_CreateColorScheme,F_Do(LOD -> Application,FM_Application_Resolve,LOD -> p_Scheme,NULL),F_Get(LOD -> Application,FA_ColorScheme));
    LOD -> DisabledScheme = (FPalette *) F_Get(LOD -> Application,FA_DisabledColorScheme);
    LOD -> Render -> Palette = LOD -> Scheme;
    
    /* rendering mode ($window-render-complex) */
    
    if (F_Do(LOD -> Render -> Application,FM_Application_ResolveInt,"$window-render-complex", TRUE))
    {
        LOD -> SYSFlags |= FF_WINDOW_SYS_COMPLEX;
    }
    else
    {
        LOD -> SYSFlags &= ~FF_WINDOW_SYS_COMPLEX;
    }

    /* refreshing mode ($window-refresh-simple) */

    if (F_Do(LOD->Render->Application, FM_Application_ResolveInt, "$window-refresh-simple", FALSE))
    {
        LOD->SYSFlags |= FF_WINDOW_SYS_REFRESH_SIMPLE;
    }
    else
    {
        LOD->SYSFlags &= ~FF_WINDOW_SYS_REFRESH_SIMPLE;
    }

    /*  This  pool  will  be  used  to  allocate  nodes  to  store   objects
    participating in the object's cycle of the window */

    LOD -> CyclePool = F_CreatePool(sizeof (FCycleNode), FA_Pool_Items,32, FA_Pool_Name,"Window.CycleChain", TAG_DONE);

    if (LOD -> CyclePool == NULL)
    {
        F_Log(FV_LOG_USER,"Unable to create CyclePool");

        return FALSE;
    }

/*** Résolution des préférences & Création des objets **********************/

    F_SuperDo(Class,Obj,FM_Frame_Setup,LOD -> Render);
    
    /* if FC_Frame didn't create the FC_ImageObject (because $window-back is
    empty or not difined), we create one ourselves */

    if (LOD -> FramePublic -> Back == NULL)
    {
        LOD -> FramePublic -> Back = ImageDisplayObject, FA_ImageDisplay_Spec,"fill", End;
            
        /* Even if the setup of the imagedisplay fails we keep the object, it
        will  fill  regions with pen fill. If we dispose the object, children
        will be unable to inherit their background, and most of the  window's
        area will never be cleared */
 
        F_Do(LOD -> FramePublic -> Back,FM_ImageDisplay_Setup,LOD -> Render);
    }

    if (LOD -> FramePublic -> Back)
    {
        F_Set(LOD -> FramePublic -> Back,FA_ImageDisplay_Origin,(uint32) &LOD -> FrameBox);
    }
    else
    {
        return FALSE;
    }

    /* the decorator needs to be created before calling MinMax() */

    data = F_Do(LOD -> Render -> Application,FM_Application_Resolve,LOD -> p_Decorator,NULL);
    
    if (data)
    {
        LOD -> Decorator = F_NewObj((STRPTR)(data),TAG_DONE);
 
        if (LOD -> Decorator)
        {
            _deco_area_public = (FAreaPublic *) F_Get(LOD -> Decorator,FA_Area_PublicData);
        }
    }

    /* Setup the root object. If the root object is a  group,  every  single
    object  of  the  group's tree is setup. If there is any problem, even in
    the deepest node of the tree, the method returns FALSE. In this case  we
    send  back  the  method  FM_Cleanup  to give a chance to objects to free
    their datas. */

    if (LOD -> Root)
    {
        if (F_Do(LOD -> Root,FM_Setup,LOD -> Render))
        {
            /* we need to call Window_Resizable() here to knob if  the  window
            is  resizable  (FA_Window_Resizable  ==  TRUE).  This attribute is
            nedeed by decorators to know if  they  have  to  create  zoom  and
            resize gadgets */

            window_resizable(Class,Obj);
        }
        else
        {
            F_Do(LOD -> Root,FM_Cleanup,LOD -> Render);

            F_Log(FV_LOG_USER,"Setup of %s{%08lx} failed",_classname(LOD -> Root),LOD -> Root);

            return FALSE;
        }
    }

    /* now that we know everything about risize capabilities  we  can  setup
    the decorator */

    if (LOD -> Decorator)
    {
        F_Do(LOD -> Decorator,FM_Connect,Obj);

        if (!F_Do(LOD -> Decorator,FM_Setup,LOD -> Render))
        {
            F_Do(LOD -> Decorator,FM_Cleanup,LOD -> Render);
            F_DisposeObj(LOD -> Decorator);

            LOD -> Decorator = NULL;
        }
    }
 
    return TRUE;
}
//+
///Window_Cleanup

/* This method is called by the  method  FM_Window_Close  just  before  the
window is closed. */

F_METHOD(void,Window_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Render)
    {
        F_DisposeObj(LOD -> PopHelp);    LOD -> PopHelp = NULL;

        F_Do(LOD -> Decorator,FM_Cleanup,LOD -> Render);
        F_DisposeObj(LOD -> Decorator);  LOD -> Decorator = NULL;
        
        _deco_area_public = NULL;
        
        F_SuperDo(Class,Obj,FM_Frame_Cleanup,LOD -> Render);
 
        F_Do(LOD -> Render -> Display,FM_RemPalette,LOD -> Scheme);
        LOD -> Scheme = NULL;

        F_Do(LOD -> Root,FM_Cleanup,LOD -> Render);
        F_DisposeObj(LOD -> Render); LOD -> Render = NULL;

        LOD -> Screen = NULL;
    }

    if (LOD -> CyclePool)
    {
        LOD -> CycleChain.Head = NULL; LOD -> EventHandlers.Head = NULL;
        LOD -> CycleChain.Tail = NULL; LOD -> EventHandlers.Tail = NULL;

        F_DeletePool(LOD -> CyclePool);  LOD -> CyclePool = NULL;
    }
}
//+
