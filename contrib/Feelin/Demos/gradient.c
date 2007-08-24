/*

   Rotating gradient with XML Application
 
*/

#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/feelin.h>

struct  FeelinBase    *FeelinBase;

struct LocalObjectData
{
   FAreaPublic                     *AreaPublic;

   FSignalHandler                   sh;

   uint32                           angle;
   STRPTR                           sspec;
   STRPTR                           espec;
   
   FObject                          image;
};

enum  {                                         // methods

      FM_Gradient_Update

      };

enum  {                                         // attributes

      FA_Gradient_Angle,
      FA_Gradient_SSpec,
      FA_Gradient_ESpec,
      FA_Gradient_Micro

      };
      
#define SPEC_FORMAT                             "<image type='gradient' angle='%ld' start='%s' end='%s' />"

///mNew
F_METHOD(uint32,mNew)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_AREA_SAVE_PUBLIC;

    LOD -> sh.Object      = Obj;
    LOD -> sh.Method      = F_IDM(FM_Gradient_Update);
    LOD -> sh.Flags       = FF_SignalHandler_Timer;
    LOD -> sh.fsh_Secs    = 0;
    LOD -> sh.fsh_Micros  = 10000;
    
    return F_SuperDo(Class,Obj,Method,

        FA_Horizontal,TRUE,
     
        Child, LOD -> image = PreviewImageObject,
            FA_Frame,         NULL,
            FA_Draggable,     FALSE,
            FA_Dropable,      FALSE,
            FA_ContextHelp,   NULL,
        End,
        
        F_IDA(FA_Gradient_Angle),   0,
        F_IDA(FA_Gradient_SSpec),   "#000000",
        F_IDA(FA_Gradient_ESpec),   "#FFFFFF",

    TAG_MORE,Msg);
}
//+
///mDispose
F_METHOD(void, mDispose)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
 
    F_Dispose(LOD->sspec); LOD->sspec = NULL;
    F_Dispose(LOD->espec); LOD->espec = NULL;
    
    F_SUPERDO();
}
//+
///mSet
F_METHOD(void,mSet)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL update = FALSE;
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Gradient_Angle:
        {
            update = TRUE;
            
            LOD -> angle = item.ti_Data;
        }
        break;
        
        case FA_Gradient_SSpec:
        {
            update = TRUE;
            
            F_Dispose(LOD->sspec);
            LOD->sspec = F_StrNew(NULL, "%s", item.ti_Data);
        }
        break;
        
        case FA_Gradient_ESpec:
        {
            update = TRUE;
            
            F_Dispose(LOD->espec);
            LOD->espec = F_StrNew(NULL, "%s", item.ti_Data);
        }
        break;
        
        case FA_Gradient_Micro:
        {
            LOD -> sh.fsh_Micros = item.ti_Data;
        }
        break;
    }

    F_SUPERDO();
    
    if (update)
    {
        F_Do(LOD -> image,FM_SetAs,FV_SetAs_String,"FA_Preview_Spec",SPEC_FORMAT,LOD -> angle,LOD -> sspec,LOD -> espec);
    }
}
//+
/// mShow
F_METHOD(LONG,mShow)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    /* It's better to add signal handlers in the FM_Show method because the
    object  may  be  hidden  at any time and even if it won't be able to be
    drawn it's a waste of time (CPU time) to leave it active. */

    if (F_SUPERDO())
    {
        F_Do(_app,FM_Application_AddSignalHandler,&LOD -> sh);

        return TRUE;
    }
    return FALSE;
}
//+
/// mHide
F_METHOD(void,mHide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (FF_SignalHandler_Active & LOD -> sh.Flags)
    {
        F_Do(_app,FM_Application_RemSignalHandler,&LOD -> sh);
    }
    F_SUPERDO();
}
//+
///mUpdate
F_METHOD(uint32,mUpdate)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> angle == 359)
    {
        LOD -> angle = 0;
    }
    else
    {
        LOD -> angle++;
    }
  
    F_Do(LOD -> image,FM_SetAs,FV_SetAs_String,"FA_Preview_Spec",SPEC_FORMAT,LOD -> angle,LOD -> sspec,LOD -> espec);
    
    return TRUE;
}
//+

/// Main
int32 main()
{
    if (F_FEELIN_OPEN)
    {
        STATIC F_METHODS_ARRAY =
        {
            F_METHODS_ADD        (mUpdate, "Update"),
            F_METHODS_ADD_STATIC (mNew, FM_New),
            F_METHODS_ADD_STATIC (mDispose, FM_Dispose),
            F_METHODS_ADD_STATIC (mSet, FM_Set),
            F_METHODS_ADD_STATIC (mShow, FM_Show),
            F_METHODS_ADD_STATIC (mHide, FM_Hide),

            F_ARRAY_END
        };

        STATIC F_ATTRIBUTES_ARRAY =
        {
            F_ATTRIBUTES_ADD("Angle", FV_TYPE_INTEGER),
            F_ATTRIBUTES_ADD("SSpec", FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("ESpec", FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("Micro", FV_TYPE_INTEGER),

            F_ARRAY_END
        };

        FClass *cc = F_CreateClass(NULL,
         
            FA_Class_Super,      FC_Group,
            FA_Class_LODSize,    sizeof (struct LocalObjectData),
            FA_Class_Attributes, F_ATTRIBUTES,
            FA_Class_Methods,    F_METHODS,
      
            TAG_DONE);
        
        if (cc)
        {
            FObject app;
            
            F_XML_DEFS_INIT(1);
            F_XML_DEFS_ADD("class:gradient",cc);
            F_XML_DEFS_DONE;
 
            app = XMLApplicationObject,

               "Source", "gradient.xml",
               "SourceType", "File",
               "Definitions", F_XML_DEFS,
               
                End;
            
            if (app)
            {
                F_Do(app, (uint32) "Run");
                F_DisposeObj(app);
            }

            F_DeleteClass(cc);
        }
        
        F_FEELIN_CLOSE;
    }
    return 0;
}
//+
