/*
 * @(#) $Header$
 *
 * BGUI library
 * systemiclass.c
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
 * Revision 42.1  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:10:26  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:17  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1999/07/31 01:55:22  mlemos
 * Fixed superclass call to dispose the objects.
 *
 * Revision 41.10  1998/02/25 21:13:18  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:54  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.

#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct {
   Object            *sd_Image;         /* The sysiclass object.      */
   struct DrawInfo   *sd_DrawInfo;      /* Current drawinfo.          */
   UWORD              sd_Which;         /* Which image to render.     */
}  SD;

///

/// OM_NEW
/*
 * Create a new object.
 */
METHOD(SystemClassNew, struct opSet *, ops)
{
   SD                *sd = INST_DATA(cl, obj);
   ULONG              rc;

   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      AsmCoerceMethod(cl, (Object *)rc, OM_SET, ops->ops_AttrList, NULL);
   };
   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Change one or more attrubutes.
 */
METHOD(SystemClassSet, struct opSet *, ops)
{
   SD                *sd = INST_DATA(cl, obj);
   struct TagItem    *tag, *tstate = ops->ops_AttrList;
   ULONG              data;
   ULONG              rc;

   /*
    * First we let the superclass
    * change the attributes it
    * knows.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   /*
    * Get the attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case VIT_BuiltIn:
         switch (data)
         {
         case BUILTIN_CHECKMARK:
            data = CHECKIMAGE;
            break;
         case BUILTIN_ARROW_LEFT:
            data = LEFTIMAGE;
            break;
         case BUILTIN_ARROW_RIGHT:
            data = RIGHTIMAGE;
            break;
         case BUILTIN_ARROW_UP:
            data = UPIMAGE;
            break;
         case BUILTIN_ARROW_DOWN:
            data = DOWNIMAGE;
            break;
         };
      case SYSIA_Which:
         sd->sd_Which    = data;
         sd->sd_DrawInfo = NULL;
         break;
      };
   };
   return rc;
}
METHOD_END
///
/// OM_GET
/*
 * Give an attribute value.
 */
METHOD(SystemClassGet, struct opGet *, opg)
{
   SD          *sd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage;

   /*
    * First we see if the attribute they want is known to us. If not
    * we pass it onto the superclass.
    */
   switch (opg->opg_AttrID)
   {
   case SYSIA_Which:
      STORE sd->sd_Which;
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };

   return rc;
}
METHOD_END
///
/// OM_DISPOSE
METHOD(SystemClassDispose, Msg, msg)
{
   SD             *sd = INST_DATA(cl, obj);

   if (sd->sd_Image) DisposeObject(sd->sd_Image);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// IM_DRAW, IM_ERASE
/*
 * Render the vector image.
 */
METHOD(SystemClassDrawErase, struct impDraw *, dr)
{
   SD            *sd = INST_DATA(cl, obj);
   ULONG          rc = 0;

   if (sd->sd_Image) DisposeObject(sd->sd_Image);

   sd->sd_Image = NewObject(NULL, "sysiclass", IA_Width,       IMAGE(obj)->Width,
                                               IA_Height,      IMAGE(obj)->Height,
                                               SYSIA_Which,    sd->sd_Which,
                                               SYSIA_DrawInfo, dr->imp_DrInfo,
                                               TAG_DONE);

   if (sd->sd_Image)
   {
      IMAGEBOX(sd->sd_Image)->Left = IMAGEBOX(obj)->Left;
      IMAGEBOX(sd->sd_Image)->Top  = IMAGEBOX(obj)->Top;

      rc = AsmDoMethodA(sd->sd_Image, (Msg)dr);
   };

   return rc;
}
METHOD_END
///

/// Class initialization.

STATIC DPFUNC ClassFunc[] = {
   IM_DRAW,                (FUNCPTR)SystemClassDrawErase,
   IM_ERASE,               (FUNCPTR)SystemClassDrawErase,
   OM_NEW,                 (FUNCPTR)SystemClassNew,
   OM_SET,                 (FUNCPTR)SystemClassSet,
   OM_GET,                 (FUNCPTR)SystemClassGet,
   OM_DISPOSE,             (FUNCPTR)SystemClassDispose,
   DF_END,                 NULL
};

makeproto Class *InitSystemClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassID, ImageClass,
                         CLASS_ObjectSize,   sizeof(SD),
                         CLASS_DFTable,      ClassFunc,
                         TAG_DONE);
}
///
