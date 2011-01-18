/*
 * @(#) $Header$
 *
 * BGUI library
 * areaclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:27:00  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:08:11  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:42  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/11/23 14:58:28  mlemos
 * Made the BaseInfo pointer passed to DoNotifyMethod be casted to GadgetInfo
 * pointer to avoid a compiler warning.
 *
 * Revision 41.10  1998/02/25 21:11:28  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:18  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

/*
 * Instance data.
 */
typedef struct {
   UWORD   ad_MinWidth;   /* Minimum width of area.  */
   UWORD   ad_MinHeight;  /* Minimum height of area. */
}  AD;
///
/// OM_NEW
METHOD(AreaClassNew, struct opSet *,ops)
{
   AD             *ad;
   struct TagItem *tags, *tstate, *tag;
   ULONG           rc;

   tags = DefTagList(BGUI_AREA_GADGET, ops->ops_AttrList);
   /*
    * Let the superclass create an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      ad = INST_DATA(cl, rc);
      
      tstate = tags;
      while (tag = NextTagItem(&tstate))
      {
         switch (tag->ti_Tag)
         {
         case AREA_MinWidth:
            ad->ad_MinWidth = tag->ti_Data;
            break;
         case AREA_MinHeight:
            ad->ad_MinHeight = tag->ti_Data;
            break;
         };
      };

      /*
       * Minimal width and height required!
       */
      if (!ad->ad_MinWidth || !ad->ad_MinHeight)
      {
         /*
          * Fail :(
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_GET
METHOD(AreaClassGet, struct opGet *,opg)
{
   AD       *ad = INST_DATA(cl, obj);
   BC       *bc = BASE_DATA(obj);
   ULONG     rc = 1;
   ULONG    *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case AREA_AreaBox:
      /*
       * Simply return a pointer to the area
       * which is computed at rendering time.
       */
      STORE &bc->bc_InnerBox;
      break;
   case AREA_MinWidth:
      STORE ad->ad_MinWidth;
      break;
   case AREA_MinHeight:
      STORE ad->ad_MinHeight;
      break;
   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END
///
/// BASE_RENDER

METHOD(AreaClassRender, struct bmRender *,bmr)
{
   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Notify the main program.
    */
   DoNotifyMethod(obj, (struct GadgetInfo *)bmr->bmr_BInfo, 0, GA_ID, GADGET(obj)->GadgetID, TAG_DONE);

   return 1;
}
METHOD_END
///
/// GM_HITTEST
/*
 * This object cannot be hit.
 */
METHOD(AreaClassHitTest, Msg, msg)
{
   return 0;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Simple dimensions request.
 */
METHOD(AreaClassDimensions, struct bmDimensions *,bmd)
{
   AD *ad = INST_DATA(cl, obj);

   /*
    * We simply add the specified minimum width and height
    * which are passed to us at create time.
    */
   return CalcDimensions(cl, obj, bmd, ad->ad_MinWidth, ad->ad_MinHeight);
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,           (FUNCPTR)AreaClassNew,
   OM_GET,           (FUNCPTR)AreaClassGet,
   BASE_RENDER,      (FUNCPTR)AreaClassRender,
   BASE_DIMENSIONS,  (FUNCPTR)AreaClassDimensions,
   GM_HITTEST,       (FUNCPTR)AreaClassHitTest,
   DF_END
};

/*
 * Simple class initialization.
 */
makeproto Class *InitAreaClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(AD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
