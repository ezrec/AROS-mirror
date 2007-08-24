/*

$VER: vs.rv (yyyy/mm/dd)

*/

/* The "Project.h" file should be generated with the F_Make command. */

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,);

F_METHOD_PROTO(void,Prefs_New);
F_METHOD_PROTO(void,Prefs_Load);
F_METHOD_PROTO(void,Prefs_Save);
//+

/*   You   can   (and   should)   remove   Class_New,   Class_Dispose   and
FV_Query_MetaTags if you don't need to create a metaclass for your class */

///Class_New
F_METHOD_NEW(Class_New)
{
    CUD = F_LOD(Class,Obj);
   
    return (FObject) F_SUPERDO();
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    CUD = F_LOD(Class,Obj);

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
                F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(<values_name>) =
            {
                F_VALUES_ADD("<name>",<value>)
           
                F_ARRAY_END
            };
   
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("<name>",<type>),
                F_ATTRIBUTES_ADD_WITH_VALUES("<name>",<type>,<values_name>),
                
                /* the two following macro should not be used */
                
                F_ATTRIBUTES_ADD_STATIC(<id>,<type>),
                F_ATTRIBUTES_ADD_STATIC_WITH_VALUES(<id>,<type>,<values_name>),
                
                /* you should always define dynamic names event if  you  use
                static ids */
                
                F_ATTRIBUTES_ADD_BOTH("<name>",<type>,<id>),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("<name>",<type>,<id>,<values_name>),
                
                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                /* to implement one of your superclass method, set <name> to
                the   full   name  of  the  method  e.g.  FM_Object_Dispose.
                Otherwise, to defined your own  method  just  name  it  e.g.
                "Multiply" */
     
                F_METHODS_ADD(<func>,"<name>"),
                
                F_METHODS_ADD_STATIC(<func>,<id>),
                F_METHODS_ADD_BOTH(<func>,"<name>",<id>"),
                
                F_ARRAY_END
            };
            
            /* Use these macros if you want to import methods or attributes
            from one of your superclasses. */
    
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("<name>"),

                F_ARRAY_END
            };
            
            STATIC F_AUTOS_ARRAY =
            {
                F_AUTOS_ADD("<name>"),
               
                F_ARRAY_END
            };
                        
            STATIC F_TAGS_ARRAY =
            {
                /* superclass name must be given  without  the  FC_  prefix
                e.g. Area instead of FC_Area */
     
                F_TAGS_ADD_SUPER(<name>),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_RESOLVES,
                F_TAGS_ADD_AUTOS,
                F_TAGS_ADD_CATALOG,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Prefs
        case FV_Query_PrefsTags:
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
