/*
 * @(#) $Header$
 *
 * BGUI library
 * indicatorclass.c
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
 * Revision 42.0  2000/05/09 22:09:12  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:25  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/03/01 19:55:15  mlemos
 * Fixed short allocation for indicator text string.
 *
 * Revision 41.10  1998/02/25 21:12:15  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:33  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.
#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct id_ {
   UWORD              id_Flags;           /* see below               */
   Object            *id_Text;            /* the text to display     */
   LONG               id_Min;             /* minimum indication      */
   LONG               id_Max;             /* maximum indication      */
   LONG               id_Level;           /* Current level           */
   UWORD              id_Justification;   /* label justification     */
   struct TextFont   *id_Font;            /* use this font           */
   UWORD              id_MinSize;         /* pre-calculated min size */
}  ID;

#define IDF_INVALID_MINSIZE  (1<<0)    /* re-calculate minsize    */

#define ID_ENTRY(tag, offset, flags) PACK_ENTRY(INDIC_TAGSTART, tag, id_, offset, flags)
#define ID_FLAG(tag, flag) PACK_LONGBIT(INDIC_TAGSTART, tag, id_, bd_Flags, PKCTRL_BIT, flag)

static ULONG IndicatorPackTable[] =
{
   PACK_STARTTABLE(INDIC_TAGSTART),

   ID_ENTRY(INDIC_Justification,    id_Justification,   PKCTRL_UWORD),
   ID_ENTRY(INDIC_Min,              id_Min,             PKCTRL_LONG),
   ID_ENTRY(INDIC_Max,              id_Max,             PKCTRL_LONG),
   ID_ENTRY(INDIC_Level,            id_Level,           PKCTRL_LONG),

   PACK_ENDTABLE
};
///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(IClassNew, struct opSet *, ops)
{
   ID             *id;
   struct TagItem *tags;
   ULONG           rc;

   tags = DefTagList(BGUI_INDICATOR_GADGET, ops->ops_AttrList);

   /*
    * Let the superclass create us an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      id = INST_DATA(cl, rc);

      /*
       * Initialize the local instance
       * data.
       */
      id->id_Justification = IDJ_LEFT;
      id->id_Max           = 100;
      id->id_Text          = BGUI_NewObject(BGUI_TEXT_GRAPHIC, TEXTA_Args, &id->id_Level, TAG_DONE);
      
      BGUI_PackStructureTags((APTR)id, IndicatorPackTable, tags);

      /*
       * are we setup OK?
       */
      if (!id->id_Text || (id->id_Min > id->id_Max))
      {
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      }
      DoSetMethodNG((Object *)rc, INDIC_FormatString, "%ld", TAG_DONE);
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Hmm, they want us out of here.
 */
METHOD(IClassDispose, Msg, msg)
{
   ID    *id = INST_DATA(cl, obj);

   /*
    * Dispose of the text graphic.
    */
   if (id->id_Text) DisposeObject(id->id_Text);

   /*
    * And let the superclass take
    * care of the rest.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Set/Update object attributes.
 */
METHOD(IClassSetUpdate, struct opUpdate *, opu)
{
   ID             *id = INST_DATA(cl, obj);
   struct TagItem *tstate = opu->opu_AttrList, *tag;
   ULONG           data;
   BOOL            update = FALSE;
   LONG            old_level = id->id_Level;
   UBYTE          *full_format;

   /*
    * First we let the superclass have a go at it.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Set attributes.
    */
   BGUI_PackStructureTags((APTR)id, IndicatorPackTable, tstate);
   
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;       
   
      switch (tag->ti_Tag)
      {
      /*
       * Intercept the font.
       */
      case BT_TextFont:
         id->id_Font = (struct TextFont *)data;
         id->id_Flags |= IDF_INVALID_MINSIZE;
         break;

      case INDIC_FormatString:
         if (full_format = BGUI_AllocPoolMem(3 + strlen((UBYTE *)data)))
         {
            sprintf(full_format, "\033l%s", data);
            DoSetMethodNG(id->id_Text, TEXTA_Text, full_format, TEXTA_CopyText, TRUE, TAG_DONE);
            BGUI_FreePoolMem(full_format);
         };
      case INDIC_Justification:
         update = TRUE;
         break;
      };
   };
   
   if (id->id_Level < id->id_Min) id->id_Level = id->id_Min;
   if (id->id_Level > id->id_Max) id->id_Level = id->id_Max;
   if (id->id_Level != old_level) update = TRUE;

   /*
    * Refresh gadget?
    */
   if (update) DoRenderMethod(obj, opu->opu_GInfo, GREDRAW_REDRAW);

   return 1;
}
METHOD_END
///
/// OM_GET
METHOD(IClassGet, struct opGet *, opg)
{
   ID          *id = INST_DATA(cl, obj);
   ULONG        rc = 1, attr = opg->opg_AttrID, *store = opg->opg_Storage;

   switch (attr)
   {
   case INDIC_FormatString:
      Get_Attr(id->id_Text, TEXTA_Text, store);
      *store += 2;
      break;

   default:
      /*
       * First we see if the attribute they want is known to us. If not
       * we pass it onto the superclass.
       */
      rc = BGUI_UnpackStructureTag((UBYTE *)id, IndicatorPackTable, attr, store);
      if (!rc) rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the object.
 */
METHOD(IClassRender, struct bmRender *, bmr)
{
   ID                *id = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   UBYTE             *str = NULL;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Setup the font.
    */
   if (id->id_Font) BSetFont(bi, id->id_Font);

   Get_Attr(id->id_Text, TEXTA_Text, (ULONG *)&str);

   if (str)
   {
      switch (id->id_Justification)
      {
      case IDJ_LEFT:
         str[1] = 'l';
         break;
      case IDJ_RIGHT:
         str[1] = 'r';
         break;
      case IDJ_CENTER:
         str[1] = 'c';
         break;
      }
      BSetDPenA(bi, TEXTPEN);
      DoMethod(id->id_Text, TEXTM_RENDER, bi, &bc->bc_InnerBox);
   };
   return 1;
}
METHOD_END
///
/// GM_HITTEST
/*
 * This object cannot be hit.
 */
METHOD(IClassHitTest, Msg, msg)
{
   return 0;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * Our parent group needs to know
 * something about our dimensions.
 */
METHOD(IClassDimensions, struct bmDimensions *, bmd)
{
   ID                *id = INST_DATA(cl, obj);
   struct RastPort   *rp = bmd->bmd_BInfo->bi_RPort;
   UWORD              mw, mh, w, h;
   LONG               old_level = id->id_Level;

   mw = mh = w = h = 0;

   /*
    * Compute size of minimum level.
    */
   id->id_Level = id->id_Min;

   DoMethod(id->id_Text, TEXTM_DIMENSIONS, rp, &mw, &mh);

   /*
    * Compute size of maximum level.
    */
   id->id_Level = id->id_Max;

   DoMethod(id->id_Text, TEXTM_DIMENSIONS, rp, &w, &h);

   if (mw < w) mw = w;
   if (mh < h) mh = h;

   id->id_Level = old_level;

   /*
    * Add the width of an average digit.
    */
   mw += TextWidthNum(rp, "0123456789", 10) / 10;

   /*
    * Setup minimum dimensions.
    */
   return CalcDimensions(cl, obj, bmd, mw, mh);
}
METHOD_END
///
/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] =
{
   BASE_RENDER,      (FUNCPTR)IClassRender,
   BASE_DIMENSIONS,  (FUNCPTR)IClassDimensions,

   OM_NEW,           (FUNCPTR)IClassNew,
   OM_SET,           (FUNCPTR)IClassSetUpdate,
   OM_UPDATE,        (FUNCPTR)IClassSetUpdate,
   OM_GET,           (FUNCPTR)IClassGet,
   OM_DISPOSE,       (FUNCPTR)IClassDispose,
   GM_HITTEST,       (FUNCPTR)IClassHitTest,
   DF_END,           NULL
};

/*
 * Simple class initialization
 */
makeproto Class *InitIndicatorClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(ID),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
