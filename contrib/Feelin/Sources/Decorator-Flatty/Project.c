/*

$VER: 01.00 (2005/08/14)
 
*/

#include "Project.h"

#ifdef __MORPHOS__
struct IntuitionBase               *IntuitionBase;
#endif

///METHODS
F_METHOD_PROTO(void, Deco_New);
F_METHOD_PROTO(void, Deco_Get);
F_METHOD_PROTO(void, Deco_Setup);
F_METHOD_PROTO(void, Deco_Cleanup);
F_METHOD_PROTO(void, Deco_Show);
F_METHOD_PROTO(void, Deco_Hide);
F_METHOD_PROTO(void, Deco_AskMinMax);
F_METHOD_PROTO(void, Deco_Layout);
F_METHOD_PROTO(void, Deco_Draw);
F_METHOD_PROTO(void, Deco_GoActive);
F_METHOD_PROTO(void, Deco_GoInactive);

F_METHOD_PROTO(void, Prefs_New);
//+
 
F_QUERY()
{
    #ifdef __MORPHOS__
    IntuitionBase = FeelinBase->Intuition;
    #endif
 
    switch (Which)
    {
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Deco_New,          FM_New),
            F_METHODS_ADD_STATIC(Deco_Get,          FM_Get),
            F_METHODS_ADD_STATIC(Deco_Setup,        FM_Setup),
            F_METHODS_ADD_STATIC(Deco_Cleanup,      FM_Cleanup),
            F_METHODS_ADD_STATIC(Deco_Show,         FM_Show),
            F_METHODS_ADD_STATIC(Deco_Hide,         FM_Hide),
            F_METHODS_ADD_STATIC(Deco_AskMinMax,    FM_AskMinMax),
            F_METHODS_ADD_STATIC(Deco_Layout,       FM_Layout),
            F_METHODS_ADD_STATIC(Deco_Draw,         FM_Draw),
            F_METHODS_ADD_STATIC(Deco_GoActive,     FM_GoActive),
            F_METHODS_ADD_STATIC(Deco_GoInactive,   FM_GoInactive),
            
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
