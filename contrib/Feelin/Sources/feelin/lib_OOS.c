/*
**    Heart of Feelin's Object-Oriented-System
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 09.00 (2005/05/18)
 
   GROG: Modifications for the new design.  Morphos  port  and  Mr  Proper.
   Replace  Forbid()/Permit()  by  ObtainSemaphore()/RelaseSemaphore(). 68k
   method call added.
   
   OLAV: Added Metaclass features.
 
$VER: 08.10 (2005/01/13)

   The whole OOS is finaly in C, even the almighty invokation  functions  :
   F_DoA(), F_ClassDoA(), F_SuperDoA().

   F_MakeObj() has been  improved.  A  taglist  can  be  added  to  objects
   generated  by  the  function.  The  taglist  comes  rigth after the last
   parameter needed to generate the object. This  allow  the  developer  to
   used the builtin collection as a basis, that can be influenced by simply
   adding attributes.

*/

#include "Private.h"

/****************************************************************************
*** Class management ********************************************************
****************************************************************************/

///f_find_class
F_LIB_FIND_CLASS
{
   uint32 key_length = F_StrLen(Name);

   if (key_length)
   {
      FHashLink *link;
      
      /*OLAV: Quitte à utiliser des sémaphore autant utiliser les bons  :-)
      Henes  sert  à  arbitrer  l'ouverture  de  la bibliothèque, celui des
      classes est contenu dans 'ClassesPool'.
      
      Note: si tu regardes les functions qui appelle F_FindClass() tu veras
      que   le   sémaphore   est   toujours   bloqué  lorsque  la  fonction
      F_FindClass() est appelée. */
  
      F_OPool(FeelinBase -> ClassesPool);
      
      link = F_HashFind(FeelinBase -> HashClasses,Name,key_length,NULL);

      F_RPool(FeelinBase -> ClassesPool);

      if (link)
      {
         return link -> Data;
      }
   }
   return NULL;
}
//+
///f_open_class
F_LIB_OPEN_CLASS
{
    FClass *cl; 

    F_OPool(FeelinBase -> ClassesPool);

    /* search with Name *as is* (needed for private classes "::FC_") */

    cl = F_FindClass(Name);

    if (!cl)
    {
        STRPTR name = F_StrNew(NULL,"Feelin/%s.fc",Name);

        /* Try to load library "<Name>.fc".*/

        if (name)
        {
            struct Library *FeelinClassBase = OpenLibrary(name,0);

            /* open library */

            if (FeelinClassBase)
            {
                struct TagItem *tags = F_Query(FV_Query_MetaClassTags,(struct FeelinBase *) FeelinBase);

                /* Before creating the class, I request  FV_Query_MetaTags.
                These tags are used to create the class of the class object
                (the meta class). */
                
                if (tags)
                {
                    STRPTR meta_name = F_StrNew(NULL,":meta:%s",Name);
      
                    struct in_FeelinClass *meta = F_NewObj(FC_Class,
                          
                          FA_Class_Super,   FC_Class,
                          FA_Class_Pool,    FALSE,
                          FA_Class_Name,    meta_name,
                          
                          TAG_MORE, tags);
                          
                    F_Dispose(meta_name);

                    if (meta)
                    {
                       //F_Log(0,"OPEN(%s) META 0x%08lx %ld",Name,meta,meta -> Public.UserCount);
                 
                       if (tags = F_Query(FV_Query_ClassTags,(struct FeelinBase *) FeelinBase))
                       {
                          if (cl = F_NewObj(meta -> Public.Name,
                                            
                             FA_Class_Name,    Name,
                             FA_Class_Module,  FeelinClassBase,
                             
                          TAG_MORE, tags))
                          {
                             // the module is saved into meta class
                             
                             OpenLibrary(name,0);
                             
                             //F_Log(0,"META 0x%08lx %ld - CLASSMETA 0x%08lx - FC_CLASS 0x%08lx",meta,meta -> Public.UserCount,_class(cl),FeelinBase -> ClassClass);

                             meta -> Module = FeelinClassBase;
                          }
                       }
                    
                       if (!cl)
                       {
                          F_DisposeObj(meta);
                       }
                    }
                }
                else
                {
                    tags = F_Query(FV_Query_ClassTags,(struct FeelinBase *) FeelinBase);

                    if (tags)
                    {
                        cl = F_NewObj(FC_Class,
                        
                            FA_Class_Name,    Name,
                            FA_Class_Module,  FeelinClassBase,
                            
                            TAG_MORE, tags);
                    }
                }

                if (!cl)
                {
                    CloseLibrary(FeelinClassBase);
                }
            }
            else
            {
                /* report opening failure to the debug console */

                F_Log(FV_LOG_USER,"F_OpenClass() Unable to open class \"%s\"",Name);
            }
            F_Dispose(name);
        }
    }

    if (cl)
    {
        cl -> UserCount++;
    }

    F_RPool(FeelinBase -> ClassesPool);

    return cl;
}
//+
///f_close_class
F_LIB_CLOSE_CLASS
{
   if (Class)
   {
      F_OPool(FeelinBase -> ClassesPool);
 
      if ((--Class -> Public.UserCount) == 0)
      {
         if (Class -> Module)
         {
            F_DeleteClass((FClass *) Class);
         }
      }

      F_RPool(FeelinBase -> ClassesPool);
   }
   return NULL;
}
//+
///f_create_classA
F_LIB_CREATE_CLASS
{
   struct TagItem *next = Tags,*item;
   FClass *cl=NULL;

   F_OPool(FeelinBase -> ClassesPool);

   while  ((item = NextTagItem(&next)))
   switch (item -> ti_Tag)
   {
      case FA_Class_Name: Name = (STRPTR)(item -> ti_Data); item -> ti_Tag = TAG_IGNORE; break;
   }

   if (Name)
   {
      cl = F_FindClass(Name);
   }

   if (cl)
   {
      F_Log(FV_LOG_DEV,"F_CreateClassA() Class '%s' already exists",Name);
   }
   else
   {
      cl = F_NewObj(FC_Class, FA_Class_Name,Name,TAG_MORE,Tags);
   }

   F_RPool(FeelinBase -> ClassesPool);

   return cl;
}
//+

/****************************************************************************
**** Invokation *************************************************************
****************************************************************************/

///f_doa
F_LIB_DO
{
   if (Obj && Method)
   {
      #ifdef DEBUG_CODE
      ///
      {
         FClass *cl;

         for (cl = (FClass *)(FeelinBase -> Classes.Head) ; cl ; cl = cl -> Next)
         {
            if (cl == _class(Obj)) break;
         }

         if (!cl)
         {
            F_Log(FV_LOG_USER,"(db)F_DoA() 0x%08lx is not an Object !! (Class 0x%08lx - Method 0x%08lx (%s))",Obj,_class(Obj),Method,f_find_method_name(Method,FeelinBase));

            return 0;
         }
      }
      //+
      #endif

      /* The following code allows Dynamic IDs (strings) to be used as well
      as  static or Resolved IDs (numerical representatio of a Dynamic ID).
      This is maybe not a good idea since, IDs  are  numeric  most  of  the
      time...  and the developer can use F_DynamicFindID() himself... maybe
      it's more convenient since Dynamic IDs is the future's key :-) */

      if (((0xFF000000 & Method) != MTHD_BASE) &&
          ((0xFF000000 & Method) != DYNA_MTHD) &&
          ((0xFF000000 & Method) != FCCM_BASE))
      {
         FClassMethod *m = F_DynamicFindMethod((STRPTR)(Method),_class(Obj),NULL);
         
         if (m)
         {
            Method = m -> ID;
         }
      }

      return F_ClassDoA(_class(Obj),Obj,Method,Msg);
   }
   return 0;
}
//+
///f_classdoa
F_LIB_CLASS_DO
/*

   This function is the heart of the invokation mechanism.

*/
{
   while (Class)
   {
      FMethod func=NULL;

      /* The methods array is prefered over the dispatcher. If a dispatcher
      is  defined,  it  is  called *only* if the method is not handled by a
      function in the array. */

      if (Class -> Methods)
      {
         FClassMethod *me;

         for (me = Class -> Methods ; me -> Function ; me++)
         {
            if (me -> ID == Method)
            {
               func = me -> Function; break;
            }
         }
      }

      /* Method has not been found in the methods table.  If  class  has  a
      dispatcher  it  is called instead. Otherwise the class is replaced by
      the super class and we try again. */

      if (!func)
      {
         func = ((struct in_FeelinClass *)(Class)) -> Dispatcher;
      }

      if (func)
      {
         uint32 prev_method = FeelinBase -> debug_classdo_method;
         FObject prev_object = FeelinBase -> debug_classdo_object;
         FClass *prev_class = FeelinBase -> debug_classdo_class;
         uint32 rc;

         /* Previous  Method/Object/Class  are  saved  before  calling  the
         method function, and restored after. These information are used by
         F_LogA() to locate message loggin. Although this  is  not  process
         relative, it should do enough in most cases (developer works). */
/*
         if (FeelinBase -> Public.Console)
         {
            F_Log(0,"CLASSDO >> %s{%08lx}.0x%08lx",Class -> Name,Obj,Method);
         }
*/
         FeelinBase -> debug_classdo_class = Class;
         FeelinBase -> debug_classdo_object = Obj;
         FeelinBase -> debug_classdo_method = Method;

         rc = F_CALL_EXTERN_METHOD(func,Class,Obj,Method,Msg,(struct FeelinBase *) FeelinBase);

         FeelinBase -> debug_classdo_class = prev_class;
         FeelinBase -> debug_classdo_object = prev_object;
         FeelinBase -> debug_classdo_method = prev_method;

         return rc;
      }
      Class = Class -> Super;
   }

   /* The method is not implemented by the  class,  or  any  of  its  super
   class, 0 is returned. */

   return 0;
}
//+
///f_superdoa
F_LIB_SUPER_DO
{

   /* F_ClassDoA() doesn't  check  is  class  is  NULL,  we  must  do  this
   ourselves */

   if (Obj && Class && Class -> Super)
   {
      return F_ClassDoA(Class -> Super,Obj,Method,Msg);
   }
   return 0;
}
//+

/****************************************************************************
*** Object management *******************************************************
****************************************************************************/

///f_new_obja
F_LIB_NEW_OBJ
{
   struct in_FeelinClass *Class = (struct in_FeelinClass *) F_OpenClass(Name);

   if (Class)
   {
      FObject Obj = F_NewP(Class -> ObjectsPool,sizeof (FClass *) + Class -> Public.Offset + Class -> Public.LODSize);

      if (Obj)
      {
         /* Class pointer is located *before* object data. The pointer need
         to be adjusted. */

         *((FClass **)(Obj)) = (FClass *)(Class);
         Obj = (FObject)((uint32)(Obj) + sizeof (FClass *));

         if (F_DoA(Obj,FM_New,Tags))
         {
            return Obj;
         }
 
         F_Do(Obj,FM_Dispose);
      }
      F_CloseClass((FClass *) Class);
   }
   return NULL;
}
//+
///f_make_obja
F_LIB_MAKE_OBJ
{
   switch (Type)
   {
      case FV_MakeObj_Label:
      {
         if (*Params++)
         {
            return TextObject,

               FA_SetMax,           FV_SetBoth,
               FA_ChainToCycle,     FALSE,
               FA_Text,             Params[0],
            
            TAG_MORE, Params + 1);
         }
         else
         {
            return TextObject,

               FA_SetMax,           FV_SetHeight,
               FA_ChainToCycle,     FALSE,
               FA_Text_HCenter,     TRUE,
               FA_Text,             Params[0],

            TAG_MORE, Params + 1);
         }
      }

      case FV_MakeObj_Button:
      {
         return TextObject,

            FA_Back,              "$button-back",
            FA_Font,              "$button-font",
            FA_Frame,             "$button-frame",
            FA_ColorScheme,       "$button-scheme",
            FA_SetMax,             FV_SetHeight,
            FA_InputMode,          FV_InputMode_Release,
            FA_Text,               Params[0],
            FA_Text_PreParse,     "<align=center>",
         
         TAG_MORE, Params + 1);
      }

      case FV_MakeObj_BarTitle:
      {
         return BarObject,

            FA_SetMax,             FV_SetHeight,
           "FA_Bar_Title",         Params[0],

         TAG_MORE, Params + 1);
      }

      case FV_MakeObj_Gauge:
      {
         return GaugeObject,

            FA_Horizontal,         Params[0],
           "FA_Numeric_Min",       Params[1],
           "FA_Numeric_Max",       Params[2],
           "FA_Numeric_Value",     Params[3],
            
         TAG_MORE, Params + 4);
      }

      case FV_MakeObj_Slider:
      {
         return SliderObject,

            FA_Horizontal,        Params[0],
           "FA_Numeric_Min",      Params[1],
           "FA_Numeric_Max",      Params[2],
           "FA_Numeric_Value",    Params[3],

         TAG_MORE, Params + 4);
      }

      case FV_MakeObj_Prop:
      {
         return PropObject,
            
            FA_Horizontal,         Params[0],
           "FA_Prop_Entries",      Params[1],
           "FA_Prop_Visible",      Params[2],
           "FA_Prop_First",        Params[3],
            
            TAG_MORE, Params + 4);
      }

      case FV_MakeObj_String:
      {
         return StringObject,
            
            FA_SetMax,             FV_SetHeight,
           "FA_String_Contents",   Params[0],
           "FA_String_MaxLen",     Params[1],
            
            TAG_MORE, Params + 2);
      }

      case FV_MakeObj_Checkbox:
      {
         return ImageObject,
            
            FA_SetMin,           FV_SetBoth,
            FA_SetMax,           FV_SetBoth,
            FA_InputMode,        FV_InputMode_Toggle,
            FA_Selected,         Params[0],
            FA_Back,            "$checkbox-back",
            FA_Frame,           "$checkbox-frame",
            FA_ColorScheme,     "$checkbox-scheme",
           "FA_Image_Spec",     "$checkbox-image",
            
            TAG_MORE, Params + 1);
      }
   }
   return NULL;
}
//+
///f_dispose_obj
F_LIB_DISPOSE_OBJ
{
   if (Obj)
   {
      struct in_FeelinClass *Class = (struct in_FeelinClass *)(_class(Obj));

      #ifdef DEBUG_CODE
///
      if (FF_DEBUG_INVOKATION & FeelinBase -> debug_flags)
      {
         FNode *node;

         for (node = FeelinBase -> Classes.Head ; node ; node = node -> Next)
         {
            if (node == (FNode *) Class) break;
         }

         if (!node)
         {
            F_Log(FV_LOG_DEV,"(db)F_DisposeObj() 0x%08lx is not an Object !! - Class 0x%08lx",Obj,Class);

            return NULL;
         }
      }
//+
      #endif

      F_Do(Obj,FM_Dispose);

      F_Dispose((APTR)((uint32)(Obj) - sizeof (FClass *)));

      F_CloseClass((FClass *) Class);
   }

   /* I always return NULL when I free a resource... it's one part  of  the
   mystery of life :-) */

   return NULL;
}
//+

/****************************************************************************
*** Extended ****************************************************************
****************************************************************************/

///f_get
F_LIB_GET
/*

   This function was  created  because  AmigE  didn't  support  va_arg  (it
   creates  A  LOT  of arrays instead). This function may seam useless now,
   but in fact its very nice. If you only need to get one attribute because
   the value used to receive the result is always cleared.

*/
{
   uint32 rc=0;

   F_Do(Obj,FM_Get,Tag,&rc,TAG_DONE);

   return rc;
}
//+
///f_set
F_LIB_SET
{
   F_Do(Obj,FM_Set,Tag,Data,TAG_DONE);
}
//+

