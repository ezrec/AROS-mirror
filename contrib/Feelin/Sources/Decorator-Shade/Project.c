/*

$VER: 02.00 (2005/08/10)
 
    Portability update.
   
    Metaclass support.
   
    Double buffered rendering totaly removed.  I  really  need  to  improve
    FC_Render support.
    
    Improved layout and rendering, previous version was hanging  if  window
    dimensions were to small.
 
$VER: 01.05 (2005/05/16)
 
    Double buffering temporarily disabled, it  causes  an  issue  with  the
    color  allocation  features  of  FC_TextDisplay,  because the FC_Render
    object created on the fly to buffer rendering  the  window  title  last
    less  than  the  FC_TextDisplay.  Also because of color allocation (and
    more) I cannot create the FC_TextDisplay object  on  the  fly  anymore,
    since colors will be loosed.
   
    I need to implement a real and efficient double buffering system,  e.i.
    FC_Render needs to be reworked. Until that buffering will be disabled.

*/

#include "Project.h"

#ifdef __MORPHOS__
struct IntuitionBase *              IntuitionBase;
#endif

struct FeelinClass                 *GadgetClass;
uint32                              FC_Gadget_Offset;
   
///METHODS
F_METHOD_PROTO(void,Gad_New);
F_METHOD_PROTO(void,Gad_Setup);
F_METHOD_PROTO(void,Gad_Cleanup);
F_METHOD_PROTO(void,Gad_Show);
F_METHOD_PROTO(void,Gad_Hide);
F_METHOD_PROTO(void,Gad_Layout);
F_METHOD_PROTO(void,Gad_Draw);
F_METHOD_PROTO(void,Gad_HandleEvent);

F_METHOD_PROTO(void,Shade_New);
F_METHOD_PROTO(void,Shade_Setup);
F_METHOD_PROTO(void,Shade_Cleanup);
F_METHOD_PROTO(void,Shade_Show);
F_METHOD_PROTO(void,Shade_Layout);
F_METHOD_PROTO(void,Shade_Draw);
F_METHOD_PROTO(void,Shade_GoActive);
F_METHOD_PROTO(void,Shade_GoInactive);

F_METHOD_PROTO(void, Prefs_New);
//+
 
///Class_New
F_METHOD_NEW(Class_New)
{
    STATIC F_METHODS_ARRAY =
    {
        F_METHODS_ADD_STATIC(Gad_New,         FM_New),
        F_METHODS_ADD_STATIC(Gad_Setup,       FM_Setup),
        F_METHODS_ADD_STATIC(Gad_Cleanup,     FM_Cleanup),
        F_METHODS_ADD_STATIC(Gad_Show,        FM_Show),
        F_METHODS_ADD_STATIC(Gad_Hide,        FM_Hide),
        F_METHODS_ADD_STATIC(Gad_Draw,        FM_Draw),
        F_METHODS_ADD_STATIC(Gad_Layout,      FM_Layout),
        F_METHODS_ADD_STATIC(Gad_HandleEvent, FM_HandleEvent),
        
        F_ARRAY_END
    };
    
    STATIC F_TAGS_ARRAY =
    {
        F_TAGS_ADD_SUPER(Area),
        F_TAGS_ADD(LODSize, sizeof (struct GAD_LocalObjectData)),
        F_TAGS_ADD_METHODS,
        
        F_ARRAY_END
    };

    #ifdef __MORPHOS__
    IntuitionBase = FeelinBase->Intuition;
    #endif
    
    GadgetClass = F_CreateClassA(NULL,F_TAGS);
 
    if (GadgetClass)
    {
       FC_Gadget_Offset = GadgetClass -> Offset;

       return (FObject) F_SUPERDO();
    }

    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   F_DeleteClass(GadgetClass); GadgetClass = NULL;
   
   F_SUPERDO();
}
//+

F_QUERY()
{
    switch (Which)
    {
///Meta
      case FV_Query_MetaClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Class_New,      FM_New),
            F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Class),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Shade_New,          FM_New),
            F_METHODS_ADD_STATIC(Shade_Setup,        FM_Setup),
            F_METHODS_ADD_STATIC(Shade_Cleanup,      FM_Cleanup),
            F_METHODS_ADD_STATIC(Shade_Show,         FM_Show),
            F_METHODS_ADD_STATIC(Shade_Layout,       FM_Layout),
            F_METHODS_ADD_STATIC(Shade_Draw,         FM_Draw),
            F_METHODS_ADD_STATIC(Shade_GoActive,     FM_GoActive),
            F_METHODS_ADD_STATIC(Shade_GoInactive,   FM_GoInactive),
            
            F_ARRAY_END
         };
          
         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Decorator),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
         
            F_ARRAY_END
         };
 
         return F_TAGS;
      }
//+
///DecoratorPrefs
        case FV_Query_DecoratorPrefsTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Prefs_New, FM_New),
                
                F_ARRAY_END
            };
                        
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreferenceGroup),
                F_TAGS_ADD_METHODS,
                
                F_ARRAY_END 
            };
            
            return F_TAGS;
        }
//+
    }
    return NULL;
}
