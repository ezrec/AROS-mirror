/*
 * @(#) $Header$
 *
 * BGUI library
 * externalclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:11:58  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:07  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * Object instance data.
 */
typedef struct {
   Object            *ed_Object;       /* Pointer to external object. */
   UWORD              ed_MinWidth;     /* Minimum width.              */
   UWORD              ed_MinHeight;    /* Minimum height.             */
   UBYTE             *ed_ClassID;      /* Public class name.          */
   Class             *ed_Class;        /* External class pointer.     */
   struct TagItem    *ed_AttrList;     /* Create time attributes.     */
   struct TagItem    *ed_TrackList;    /* Tracked attributes.         */
   UWORD              ed_Flags;        /* See below.                  */
   WORD               ed_W, ed_H;      /* Width and height of gadget. */
} ED;

#define EDF_NO_REBUILD     (1<<0)   /* No need to rebuild the object. */

/*
 * Module prototypes.
 */
SAVEDS ASM STATIC ULONG ExtClassDispatch( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );

/*
 * Class initialization.
 */
makeproto Class *InitExtClass( void )
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(ED),
                         CLASS_Dispatcher,     (ULONG)ExtClassDispatch,
                         TAG_DONE);
}

/*
 * Build the create tags and tracked tags.
 */
STATIC ASM BOOL SetupAttrList(REG(a0) ED *ed, REG(a1) struct TagItem *attr)
{
   struct TagItem *tstate, *tag;

   /*
    * Clone the original tag-list.
    */
   if (ed->ed_AttrList = CloneTagItems(attr))
   {
      tstate = ed->ed_AttrList;

      /*
       * Set up attributes for tracking.
       */
      while (tag = NextTagItem(&tstate))
      {
         if (tag->ti_Tag == EXT_TrackAttr)
         {
            tag->ti_Tag = tag->ti_Data;
         }
         else
         {
            tag->ti_Tag = TAG_IGNORE;
         };
      }
      /*
       * Allocate the tracked tag array.
       */
      if (ed->ed_TrackList = CloneTagItems(ed->ed_AttrList))
      {
         RefreshTagItemClones(ed->ed_AttrList, attr);

         return TRUE;
      };
   };
   return FALSE;
}

/*
 * Set the track changes.
 */
STATIC ASM VOID GetTrackChanges( REG(a0) ED *ed )
{
   struct TagItem    *tstate = ed->ed_TrackList, *tag;

   /*
    * Get the attributes to track in the new object.
    */
   while (tag = NextTagItem(&tstate))
      AsmDoMethod(ed->ed_Object, OM_GET, tag->ti_Tag, &tag->ti_Data);
}

/*
 * Setup object size.
 */
STATIC ASM VOID SetupSize(REG(a0) Class *cl, REG(a1) Object *obj)
{
   ED                 *ed = INST_DATA(cl, obj);
   BC                 *bc = BASE_DATA(obj);
   struct IBox        *box = &bc->bc_InnerBox;
   struct TagItem     *tstate = ed->ed_AttrList, *tag;
   /*
    * Set values in the taglist.
    */
   while (tag = NextTagItem(&tstate))
   {
      switch (tag->ti_Tag)
      {
      case GA_Left:   tag->ti_Data = box->Left;   break;
      case GA_Top:    tag->ti_Data = box->Top;    break;
      case GA_Width:  tag->ti_Data = box->Width;  break;
      case GA_Height: tag->ti_Data = box->Height; break;
      case GA_Bounds: tag->ti_Data = (ULONG)box;  break;
      }
   }
}


/*
 * Class dispatcher.
 */
SAVEDS ASM STATIC ULONG ExtClassDispatch( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
{
   ED                *ed = INST_DATA(cl, obj);
   BC                *bc;
   struct TagItem    *attr;
   BOOL               set;
   ULONG              rc;
   struct BaseInfo   *bi;

   /*
    * What have we got here...
    */
   switch (msg->MethodID)
   {
   case OM_NEW:
      /*
       * Let the superclass make the object.
       */
      if (rc = AsmDoSuperMethodA(cl, obj, msg))
      {
         /*
          * Get to the instance data.
          */
         ed = INST_DATA(cl, rc);

         /*
          * Get tag pointer.
          */
         attr = ((struct opSet *)msg)->ops_AttrList;

         /*
          * Initialize attributes.
          */
         ed->ed_MinWidth   = GetTagData(EXT_MinWidth,  0, attr);
         ed->ed_MinHeight  = GetTagData(EXT_MinHeight, 0, attr);
         ed->ed_ClassID    = (UBYTE *)GetTagData(EXT_ClassID, NULL, attr);
         ed->ed_Class      = (Class *)GetTagData(EXT_Class,   NULL, attr);
         ed->ed_Flags      = GetTagData(EXT_NoRebuild, FALSE, attr) ? EDF_NO_REBUILD : 0;

         /*
          * Do we have a class pointer or ID?
          */
         if (ed->ed_ClassID || ed->ed_Class)
         {
            /*
             * Rebuild?
             */
            if (ed->ed_Flags & EDF_NO_REBUILD)
            {
               if (ed->ed_Object = NewObjectA(ed->ed_Class, ed->ed_Class ? NULL : ed->ed_ClassID, attr))
               {
                  DoSetMethodNG(ed->ed_Object, ICA_TARGET, ICTARGET_IDCMP, TAG_END);
                  return rc;
               }
            }
            else
            {
               /*
                * Setup attribute list.
                */
               if (SetupAttrList(ed, attr))
               {
                  return rc;
               };
            };
         };
         /*
          * Oops... Screwup.
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      }
      break;

   case OM_DISPOSE:
      /*
       * Dispose the object.
       */
      if (ed->ed_Object) AsmDoMethod(ed->ed_Object, OM_DISPOSE);

      /*
       * Free the attrlist and the tracked attributes.
       */
      FreeTagItems(ed->ed_AttrList);
      FreeTagItems(ed->ed_TrackList);

      /*
       * The rest is done by the superclass...
       */
      rc = AsmDoSuperMethodA(cl, obj, msg);
      break;

   case GM_RENDER:
      /*
       * We ignore it for now.
       */
      rc = AsmDoSuperMethodA(cl, obj, msg);
      break;

   case BASE_RENDER:
      /*
       * Render the baseclass.
       */
      AsmDoSuperMethodA(cl, obj, msg);

      bc = BASE_DATA(obj);

      /*
       * Then the object.
       */
      if (!(ed->ed_Flags & EDF_NO_REBUILD))
      {
         if ((ed->ed_W != bc->bc_InnerBox.Width) || (ed->ed_H != bc->bc_InnerBox.Height))
         {
            ed->ed_W = bc->bc_InnerBox.Width;
            ed->ed_H = bc->bc_InnerBox.Height;
            set = FALSE;

            if (ed->ed_Object)
            {
               GetTrackChanges(ed);
               set = TRUE;
               AsmDoMethod(ed->ed_Object, OM_DISPOSE);
               ed->ed_Object = NULL;
            };

            SetupSize(cl, obj);

            /*
             * Create a new object.
             */
            if (ed->ed_Object = NewObjectA(ed->ed_Class, ed->ed_Class ? NULL : ed->ed_ClassID, ed->ed_AttrList))
               DoSetMethodNG(ed->ed_Object, ICA_TARGET, ICTARGET_IDCMP, set ? TAG_MORE : TAG_DONE, ed->ed_TrackList);
         };
      };
      if (ed->ed_Object)
      {
         SetGadgetBounds(ed->ed_Object, &bc->bc_InnerBox);

         bi = ((struct bmRender *)msg)->bmr_BInfo;
         rc = AsmDoMethod(ed->ed_Object, GM_RENDER, bi, bi->bi_RPort, ((struct bmRender *)msg)->bmr_Flags);
      }
      else
      {
         DisplayBeep(NULL);
         rc = 0;
      };
      break;

   case OM_GET:
      /*
       * What do they want?
       */
      rc = 1;
      switch (((struct opGet *)msg)->opg_AttrID)
      {
      case EXT_Object:
         *(((struct opGet *)msg)->opg_Storage) = (ULONG)ed->ed_Object;
         break;

      default:
         rc = AsmDoSuperMethodA(cl, obj, msg);

         if (ed->ed_Object)
            rc = AsmDoMethodA(ed->ed_Object, msg);
         break;
      }
      break;

   case GM_HITTEST:
      rc = AsmDoSuperMethodA(cl, obj, msg);
      /*
       * Are we hit?
       */
      if (rc != GMR_GADGETHIT) break;
      /*
       * Fall through (and forward).
       */
   case GM_GOACTIVE:
   case GM_HANDLEINPUT:
      /*
       * Forward message.
       */
      if (ed->ed_Object) rc = ForwardMsg(obj, ed->ed_Object, msg);
      break;

   case BASE_DIMENSIONS:
      /*
       * Setup minimum size.
       */
      rc = CalcDimensions(cl, obj, (struct bmDimensions *)msg, ed->ed_MinWidth, ed->ed_MinHeight);
      break;

   default:
      /*
       * First we go.
       */
      AsmDoSuperMethodA(cl, obj, msg);

      /*
       * Pass the message to the object.
       */
      if (ed->ed_Object)
         rc = AsmDoMethodA(ed->ed_Object, msg);
      break;
   }
   return rc;
}
