/*
 * @(#) $Header$
 *
 * BGUI library
 * infoclass.c
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
 * Revision 42.1  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.0  2000/05/09 22:09:14  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:27  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/11/29 22:51:52  mlemos
 * Removed needless code of OM_GET.
 *
 * Revision 41.10  1998/02/25 21:12:18  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:37  mlemos
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
   ULONG              id_Flags;     /* See below.                         */
   Object            *id_Text;      /* Text graphic.                      */
   UWORD              id_MinLines;  /* Minimum Number of lines displayed. */
}  ID;

#define IDF_FIXWIDTH       (1<<0)   /* Width not smaller than the text.   */
#define IDF_FIXHEIGHT      (1<<1)   /* Width not smaller than the text.   */

///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(InfoClassNew, struct opSet *, ops)
{
   ID                *id;
   struct TagItem    *tags;
   ULONG              rc;
   ULONG              ho, vo;

   tags = DefTagList(BGUI_INFO_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass setup an object for us.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      id = INST_DATA(cl, rc);

      if (id->id_Text = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_CopyText, TRUE, TAG_DONE))
      {
         /*
          * Pick up frame offsets.
          */
         ho = GetTagData(INFO_HorizOffset, 8, tags);
         vo = GetTagData(INFO_VertOffset,  6, tags);

         if (ho) ho--;
         if (vo) vo--;

         DoSetMethodNG((Object *)rc, BT_LeftOffset, ho, BT_RightOffset, ho, BT_TopOffset, vo, BT_BottomOffset, vo,
                                     TAG_MORE, tags);
      }
      else
      {
         /*
          * Failure!
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = NULL;
      };
   }
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Change the object attributes.
 */
METHOD(InfoClassSetUpdate, struct opSet *, ops)
{
   ID              *id = INST_DATA(cl, obj);
   struct TagItem  *tstate = ops->ops_AttrList, *tag;
   BOOL             vc = FALSE;
   ULONG            data;

   /*
    * First we let the superclass have a go at it.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)ops);

   /*
    * Change attributes we know.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case INFO_TextFormat:
         vc = TRUE;
         DoSetMethodNG(id->id_Text, TEXTA_Text, data, TAG_DONE);
         break;

      case INFO_Args:
         vc = TRUE;
         DoSetMethodNG(id->id_Text, TEXTA_Args, data, TAG_DONE);
         break;

      case INFO_MinLines:
         id->id_MinLines = data;
         break;

      case INFO_FixTextWidth:
         if (data) id->id_Flags |=  IDF_FIXWIDTH;
         else      id->id_Flags &= ~IDF_FIXWIDTH;
         break;

      case INFO_FixTextHeight:
         if (data) id->id_Flags |=  IDF_FIXHEIGHT;
         else      id->id_Flags &= ~IDF_FIXHEIGHT;
         break;
      };
   };

   /*
    * Sanity check.
    */
   if (id->id_MinLines < 1) id->id_MinLines = 1;

   /*
    * Change visually?
    */
   if (vc && ops->ops_GInfo) DoRenderMethod(obj, ops->ops_GInfo, GREDRAW_REDRAW);

   return 1;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the object.
 */
METHOD(InfoClassRender, struct bmRender *, bmr)
{
   ID              *id = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   struct BaseInfo *bi = bmr->bmr_BInfo;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Setup the font.
    */
   if (bc->bc_TextFont) BSetFont(bi, bc->bc_TextFont);

   /*
    * Render the text.
    */
   BSetDPenA(bi, TEXTPEN);
   DoMethod(id->id_Text, TEXTM_RENDER, bi, &bc->bc_InnerBox);

   return 1;
}
METHOD_END
///
/// GM_HITTEST
/*
 * We do not respond to clicking.
 */
METHOD(InfoClassHitTest, struct gpHitTest *, gph)
{
    return 0;
}
METHOD_END
///
/// OM_DIPOSE
/*
 * Dispose of ourselves.
 */
METHOD(InfoClassDispose, Msg, msg)
{
   ID         *id = INST_DATA(cl, obj);

   /*
    * Free the text graphic.
    */
   if (id->id_Text) DisposeObject(id->id_Text);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want our dimensions.
 */
METHOD(InfoClassDimensions, struct bmDimensions *, bmd)
{
   ID              *id = INST_DATA(cl, obj);
   BC              *bc = BASE_DATA(obj);
   struct BaseInfo *bi = bmd->bmd_BInfo;
   UWORD            mw, mh;

   /*
    * Setup font.
    */
   if (bc->bc_TextFont) BSetFont(bi, bc->bc_TextFont);

   DoMethod(id->id_Text, TEXTM_DIMENSIONS, bi->bi_RPort, &mw, &mh);

   /*
    * Fix text size?
    */
   if (!(id->id_Flags & IDF_FIXWIDTH))  mw = 16;
   if (!(id->id_Flags & IDF_FIXHEIGHT)) mh = id->id_MinLines * bi->bi_RPort->TxHeight;

   return CalcDimensions(cl, obj, bmd, mw, mh);
}
METHOD_END
///
/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,      (FUNCPTR)InfoClassRender,
   BASE_DIMENSIONS,  (FUNCPTR)InfoClassDimensions,

   OM_NEW,           (FUNCPTR)InfoClassNew,
   OM_SET,           (FUNCPTR)InfoClassSetUpdate,
   OM_UPDATE,        (FUNCPTR)InfoClassSetUpdate,
   OM_DISPOSE,       (FUNCPTR)InfoClassDispose,
   GM_HITTEST,       (FUNCPTR)InfoClassHitTest,
   DF_END,            NULL,
};

/*
 * Simple class initialization.
 */
makeproto Class *InitInfoClass( void )
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(ID),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///

