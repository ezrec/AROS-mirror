/*
**    $VER: fc_Class.c 09.00 (2005/07/06)
**
**    Class class :-)
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

 * 09.00 ** 2005/07/06 *

   GROG: Modifications for the new design. Morphos port and Mr Proper.

   OLAV:  It's  now  possible,  forced  in  fact  since  FA_Class_Init  and
   FA_Class_Exit  are considered deprecated, to create metaclasses (classes
   for classes).

 * 08.06 ** 2005/04/06 *

   Memory  pool  is  disabled  for  custom  classes.  The   new   attribute
   FA_Class_Module  is  used  to  provide  the  library  base of the class,
   instead of writting  it  direcly  in  the  structure.  More  over,  this
   attribute can be used to get the library base of the class.

 * 08.04 ** 2005/01/28 *

   A message is logged when an unknown attribute is supplied. This  doesn't
   do much, but can be very useful for other people who, like me, forget to
   terminate their taglist with a TAG_DONE.

   The functions defined by FA_Class_Init and FA_Class_Exit now  receive  a
   pointer to FeelinBase in A6.

   When the attribute FA_Class_Pool is NULL the default memory pool  should
   be  used  to create object. Formely I checked the class pool everytime I
   created / deleted objects. Now I set the right pool pointer in "Class ->
   ObjectsPool".  Now  I  only  need  to  check the pool when I dispose the
   class.

*/

#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///f_localize
STATIC struct Catalog * f_localize
(
   STRPTR CatalogName,
   struct FeelinCatalogEntry *Table,
   struct in_FeelinBase *FeelinBase
)
/*

  This function is used to replace default class strings by localeized ones

*/
{
   struct FeelinCatalogEntry *en;

   for (en = Table ; en -> ID ; en++)
   {
      en -> String = NULL;
   }

   if (CatalogName)
   {
      if (LocaleBase)
      {
         struct Catalog *cat = OpenCatalog(NULL,CatalogName,TAG_DONE);

         if (cat)
         {
            for (en = Table ; en -> ID ; en++)
            {
               en -> String = GetCatalogStr(cat,en -> ID,en -> Default);
            }

            return cat;
         }
      }
   }

/* default */

   for (en = Table ; en -> ID ; en++)
   {
      en -> String = en -> Default;
   }
   return NULL;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Class_New
F_METHOD_NEW(Class_New)
{
    struct in_FeelinClass *LOD = F_LOD(Class,Obj);
    const struct TagItem *Tags = Msg;
    struct TagItem *item;
    FCatalogEntry *cat_table=NULL;
    STRPTR classname=NULL,supername=NULL,cat_name=NULL;
    BOOL haspool=TRUE;

    while  ((item = NextTagItem(&Tags)))
    switch (item -> ti_Tag)
    {
        case FA_Class_Name:              classname = (STRPTR) item -> ti_Data; break;
        case FA_Class_Super:             supername = (STRPTR) item -> ti_Data; break;
        case FA_Class_LODSize:           LOD -> Public.LODSize    = (((item -> ti_Data + 3) >> 2) << 2); break;
        case FA_Class_Dispatcher:        LOD -> Dispatcher        = (FMethod) item -> ti_Data; break;
        case FA_Class_UserData:          LOD -> Public.UserData   = (APTR) item -> ti_Data; break;
        case FA_Class_Pool:              haspool = item -> ti_Data; break;
        case FA_Class_Methods:           LOD -> Public.Methods         = (APTR) item -> ti_Data; break;
        case FA_Class_Attributes:        LOD -> Public.Attributes      = (APTR) item -> ti_Data; break;
        case FA_Class_ResolveTable:      LOD -> Public.ResolvedIDs     = (APTR) item -> ti_Data; break;
        case FA_Class_AutoResolveTable:  LOD -> Public.AutoResolvedIDs = (APTR) item -> ti_Data; break;
        case FA_Class_CatalogName:       cat_name = (STRPTR) item -> ti_Data; break;
        case FA_Class_CatalogTable:      cat_table = (APTR) item -> ti_Data; break;
        case FA_Class_Module:            LOD -> Module = (struct Library *)(item -> ti_Data); break;

        default:
        {
           F_Log(0,"Unknow Attribute : 0x%08lx (0x%08lx) class (%s) super (%s)",item -> ti_Tag,item -> ti_Data,classname,supername);
        }
    }

    if (classname)
    {
        LOD -> Public.Name = F_StrNew(NULL,"%s",classname);
    }
    else
    {
        LOD -> Public.Name = F_StrNew(NULL,":cc:%08lx%02lx",LOD,LOD -> Public.LODSize);
    }

    if (!LOD -> Public.Name)
    {
        F_Log(FV_LOG_USER,"Unable to create class name"); goto __error;
    }

    if (supername)
    {
        FClass *super = F_OpenClass(supername);

        if (super)
        {
            LOD -> Public.Super  = super;
            LOD -> Public.Offset = super -> Offset + super -> LODSize;
        }
        else goto __error;
    }

/*** memory pool setup *****************************************************/

    /*OLAV-20050605: if FA_Class_LODSize equals 0 no pool is created */

    if (haspool && classname && LOD -> Public.LODSize)
    {
        LOD -> ObjectsPool = F_CreatePool(sizeof (APTR) + LOD -> Public.Offset + LOD -> Public.LODSize,FA_Pool_Name,LOD -> Public.Name,TAG_DONE);
    }

    if (!LOD -> ObjectsPool)
    {
        if (LOD -> Public.Super)
        {
            LOD -> ObjectsPool = ((struct in_FeelinClass *)(LOD -> Public.Super)) -> ObjectsPool;

            LOD -> Flags |= FF_CLASS_INHERITED_POOL;
        }
        else
        {
            LOD -> ObjectsPool = FeelinBase -> DefaultPool;

            LOD -> Flags |= FF_CLASS_INHERITED_POOL;
        }
    }

/*** localization **********************************************************/

    if (cat_table)
    {
       STRPTR str;

       if (cat_name)
       {
          str = F_StrNew(NULL,"Feelin/%s.catalog",cat_name);
       }
       else
       {
          str = F_StrNew(NULL,"Feelin/FC_%s.catalog",LOD -> Public.Name);
       }

       if (str)
       {
          LOD -> Catalog = f_localize(str,cat_table,FeelinBase);

          F_Dispose(str);
       }
    }

/*** linking ***************************************************************/

    F_HashAdd(FeelinBase -> HashClasses,LOD -> Public.Name,F_StrLen(LOD -> Public.Name),Obj);
    F_LinkTail(&FeelinBase -> Classes,(FNode *) LOD);

/*** dynamic IDs setup *****************************************************/

    if (!F_DynamicCreate((FClass *) LOD))
    {
       F_Log(FV_LOG_USER,"(%s) Unable to create Dynamic entries",(LOD -> Module) ? LOD -> Module -> lib_Node.ln_Name : "custom");

       goto __error;
    }

    return Obj;

__error:

    return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   struct in_FeelinClass *LOD = F_LOD(Class,Obj);

//   F_Log(0,"Class 0x%08lx (%s)(0x%08lx,%s)",Obj,LOD -> Public.Name,Class,Class -> Name);

   F_OPool(FeelinBase -> ClassesPool);

   /* Check User Count */

   if (LOD -> Public.UserCount)
   {
      F_Alert("Class.Dispose()","!! WARNING !!\nOpen count of '%s' at %ld\nClass will be removed anyway",LOD -> Public.Name,LOD -> Public.UserCount);
   }

   if (LOD -> Catalog)
   {
      CloseCatalog(LOD -> Catalog); LOD -> Catalog = NULL;
   }

   /* unlink class */

   F_HashRem(FeelinBase -> HashClasses,LOD -> Public.Name,F_StrLen(LOD -> Public.Name));

   if (F_LinkMember(&FeelinBase -> Classes,(FNode *) LOD))
   {
      F_LinkRemove(&FeelinBase -> Classes,(FNode *) LOD);
   }

   /* remove dynamic entries */

   F_DynamicDelete((FClass *) LOD);

   if (!(FF_CLASS_INHERITED_POOL & LOD -> Flags))
   {
      F_DeletePool(LOD -> ObjectsPool); LOD -> ObjectsPool = NULL;
   }

   F_Dispose(LOD -> Public.Name);

   /* automatically close library module, if class was built from a library
   */

   if (LOD -> Module)
   {
      CloseLibrary(LOD -> Module); LOD -> Module = NULL;
   }

   /* Free SuperClass */

   F_CloseClass(LOD -> Public.Super);

   F_RPool(FeelinBase -> ClassesPool);
}
//+

/****************************************************************************
*** Constructor *************************************************************
****************************************************************************/

STATIC F_METHODS_ARRAY =
{
   F_METHODS_ADD_BOTH(Class_New,       "New",      FM_New),
   F_METHODS_ADD_BOTH(Class_Dispose,   "Dispose",  FM_Dispose),

   F_ARRAY_END
};

///fc_class_create
FClass * fc_class_create(struct in_FeelinBase *FeelinBase)
{
   struct in_FeelinClass *cl = F_NewP(FeelinBase -> ClassesPool,sizeof (FClass *) + sizeof (struct in_FeelinClass));

   if (cl)
   {
      *(FClass **)(cl) = (FClass *)((ULONG)(cl) + sizeof (FClass *));
      cl = (struct in_FeelinClass *)((ULONG)(cl) + sizeof (FClass *));

      FeelinBase -> ClassClass = (FClass *) cl;

      cl -> Public.Name = FC_Class;
      cl -> Public.LODSize = sizeof (struct in_FeelinClass);
      cl -> Public.UserCount = 1;
      cl -> Public.Methods = F_METHODS;
      cl -> ObjectsPool = FeelinBase -> ClassesPool;

      F_LinkTail(&FeelinBase -> Classes,(FNode *) cl);
      F_HashAdd(FeelinBase -> HashClasses,cl -> Public.Name,F_StrLen(cl -> Public.Name),cl);
   }
   return (FClass *)(cl);
}
//+

