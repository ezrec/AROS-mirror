/*
 * @(#) $Header$
 *
 * BGUI library
 * mxclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.5  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.4  2003/01/18 19:10:00  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:02  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:09:48  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:49  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/11/22 00:31:42  mlemos
 * Made the radio buttons notifications map GA_Selected into TAG_IGNORE to not
 * set the MX class group selected attribute.
 *
 * Revision 41.10  1998/02/25 21:12:45  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:16  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/// Class definitions.
/*
 * Object instance data.
 */
typedef struct {
   UBYTE            **md_LabelStrings;    /* MX button labels.             */
   ULONG              md_LabelStringsID;  /* Starting string ID label.     */
   LONG               md_ActiveLabel;     /* Currently active selection.   */
   ULONG              md_MaxLabel;        /* Number of available labels.   */
   ULONG              md_Place;           /* Place of the labels.          */
   Object           **md_Objects;         /* MX buttons.                   */
   struct TextAttr   *md_TabsFont;        /* Font for the buttons.         */
   UWORD              md_Type;            /* MX Type.                      */
   ULONG              md_BackFill[2];     /* Back fill tag.                */
}  MD;

/*
 * Notification map.
 */
STATIC ULONG id2act[] = {
   GA_UserData,      MX_Active,
   GA_Selected,      TAG_IGNORE,
   TAG_END
};

/*
 * Module prototypes.
 */
METHODPROTO(MXClassSetUpdate,   struct opUpdate *, );
///
/// AddMXObjects()
/*
 * Add MX objects.
 */
//STATIC ASM BOOL AddMXObjects(REG(a0) Object *target, REG(a1) MD *md)
STATIC ASM REGFUNC2(BOOL, AddMXObjects,
	REGPARAM(A0, Object *, target),
	REGPARAM(A1, MD *, md))
{
   UBYTE       **labels = md->md_LabelStrings;
   UWORD         i = 0;
   ULONG         id = md->md_LabelStringsID;
   BOOL          rc = FALSE, tab;
   ULONG         tags[32], *t = tags;

   static ULONG  tags_fix[] = { LGO_FixMinWidth, TRUE, LGO_FixMinHeight, TRUE, TAG_DONE };

   *t++ = GA_ToggleSelect;       *t++ = TRUE;
   *t++ = GA_ID;                 *t++ = GADGET(target)->GadgetID;
   *t++ = GA_Disabled;           *t++ = GADGET(target)->Flags & GFLG_DISABLED;
   *t++ = LAB_SelectedDriPen;    *t++ = TEXTPEN;

   switch (md->md_Type)
   {
   case MXTYPE_RADIOBUTTON:
      *t++ = LAB_Place;          *t++ = md->md_Place;

      tab = FALSE;
      break;

   case MXTYPE_TAB_TOP:
      *t++ = FRM_Type;           *t++ = FRTYPE_TAB_TOP;
      goto common_tab;

   case MXTYPE_TAB_BOTTOM:
      *t++ = FRM_Type;           *t++ = FRTYPE_TAB_BOTTOM;
      goto common_tab;

   case MXTYPE_TAB_LEFT:
      *t++ = FRM_Type;           *t++ = FRTYPE_TAB_LEFT;
      goto common_tab;

   case MXTYPE_TAB_RIGHT:
      *t++ = FRM_Type;           *t++ = FRTYPE_TAB_RIGHT;

   common_tab:
      *t++ = BUTTON_SelectOnly;  *t++ = TRUE;
      *t++ = LAB_Place;          *t++ = PLACE_IN;
      *t++ = LAB_SelectedStyle;  *t++ = FSF_BOLD;
      *t++ = LAB_TextAttr;       *t++ = (ULONG)md->md_TabsFont;
      *t++ = md->md_BackFill[0]; *t++ = md->md_BackFill[1];

      tab = TRUE;
      break;

   default:
      return FALSE;
   };
   *t++ = TAG_DONE;

   /*
    * Allocate an array to hold the object pointers.
    */
   if (md->md_Objects = (Object **)BGUI_AllocPoolMem((md->md_MaxLabel + 1) * sizeof(Object *)))
   {
      /*
       * Loop through all labels.
       */
      while (*labels)
      {
         /*
          * Create a toggle object.
          */
         if (md->md_Objects[i] = BGUI_NewObject(tab ? BGUI_BUTTON_GADGET : BGUI_RADIOBUTTON_GADGET,
                  LAB_Label,       *labels,
                  LAB_LabelID,      id,
                  GA_UserData,      i,
                  GA_Selected,      (i == md->md_ActiveLabel),
                  TAG_MORE,         tags))
         {
            /*
             * Add it to the parent group.
             */
            if (AsmDoMethod(target, GRM_ADDMEMBER, md->md_Objects[i], tab ? TAG_DONE : TAG_MORE, &tags_fix))
            {
               if (AsmDoMethod(md->md_Objects[i], BASE_ADDMAP, target, id2act))
               {
                  i++;
                  labels++;
                  if (id) id++;
               }
               else
                  goto addmapError;
            }
            else
            {
               addmapError:
               /*
                * Adding failed.
                */
               DisposeObject(md->md_Objects[i]);
               goto addError;
            }
         } else
            goto addError;
      }
      rc = TRUE;
   }
   addError:
   
   return rc;
}
REGFUNC_END
///
/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(MXClassNew, struct opSet *, ops)
{
   MD             *md;
   struct TagItem *tstate, *tags = ops->ops_AttrList, *tag;
   ULONG           rc, data;
   int             type = MXTYPE_RADIOBUTTON;

   /*
    * No labels? No gadget!
    */
   if (!FindTagItem(MX_Labels, tags))
      return NULL;

   tags = DefTagList(BGUI_MX_GADGET, tags);

   if (GetTagData(MX_TabsObject, FALSE, tags))
   {
      if (GetTagData(MX_TabsUpsideDown, FALSE, tags))
         type = MXTYPE_TAB_BOTTOM;
      else
         type = MXTYPE_TAB_TOP;
   }
   type = GetTagData(MX_Type, type, tags);

   /*
    * Let the superclass setup an object for us.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Initialize instance data.
       */
      md                 = INST_DATA(cl, rc);
      md->md_Type        = type;
      md->md_BackFill[0] = TAG_IGNORE;
      md->md_Place       = PLACE_RIGHT;

      tstate = tags;
      while (tag = NextTagItem(&tstate))
      {
         data = tag->ti_Data;
         switch (tag->ti_Tag)
         {
         case MX_Spacing:
            /*
             * If we are a regular mx object we need a default spacing.
             */
            if (type == MXTYPE_RADIOBUTTON)
               DoSuperSetMethodNG(cl, (Object *)rc, GROUP_Spacing, data, TAG_DONE);
            break;

         case MX_LabelPlace:
            /*
             * Only PLACE_LEFT & PLACE_RIGHT are supported.
             */
            if ((md->md_Place == PLACE_LEFT) || (md->md_Place == PLACE_RIGHT))
               md->md_Place = data;
            break;

         case MX_Labels:
            md->md_LabelStrings = (UBYTE **)data;
            break;

         case MX_LabelsID:
            md->md_LabelStringsID = data;
            break;

         case MX_TabsTextAttr:
            md->md_TabsFont = (struct TextAttr *)data;
            break;

         case MX_TabsBackFill:
            md->md_BackFill[0] = FRM_BackFill;
            md->md_BackFill[1] = data;
            break;

         case MX_TabsBackPen:
            md->md_BackFill[0] = FRM_BackPen;
            md->md_BackFill[1] = data;
            break;

         case MX_TabsBackDriPen:
            md->md_BackFill[0] = FRM_BackDriPen;
            md->md_BackFill[1] = data;
            break;

         case MX_TabsBackFillHook:
            md->md_BackFill[0] = FRM_BackFillHook;
            md->md_BackFill[1] = data;
            break;

         case MX_TabsFillPattern:
            md->md_BackFill[0] = FRM_FillPattern;
            md->md_BackFill[1] = data;
            break;
         }
      }

      /*
       * Count the number of labels.
       */
      md->md_MaxLabel = CountLabels(md->md_LabelStrings);

      /*
       * Sanity check.
       */
      if (md->md_ActiveLabel < 0)
      {
         if (type) md->md_ActiveLabel = md->md_MaxLabel;
         else      md->md_ActiveLabel = 0;
      };
      if (md->md_ActiveLabel > md->md_MaxLabel)
      {
         if (type) md->md_ActiveLabel = 0;
         else      md->md_ActiveLabel = md->md_MaxLabel;
      };

      if (AddMXObjects((Object *)rc, md))
      {
         AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL);
      }
      else
      {
         /*
          * Oops, something screwed up.
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
/// OM_SET, OM_UPDATE
/*
 * Change the object attributes.
 */
METHOD(MXClassSetUpdate, struct opUpdate *, opu)
{
   MD                *md = INST_DATA(cl, obj);
   struct GadgetInfo *gi = opu->opu_GInfo;
   struct TagItem    *tstate = opu->opu_AttrList, *tag, ttag[2];
   struct opSet       set;
   ULONG              data, i;

   set.MethodID     = OM_SET;
   set.ops_AttrList = ttag;
   set.ops_GInfo    = gi;
   ttag[1].ti_Tag   = TAG_DONE;

   /*
    * First let the superclass do its thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   /*
    * Make sure that we are GACT_IMMEDIATE.
    */
   GADGET(obj)->Activation |= GACT_IMMEDIATE;
   GADGET(obj)->Activation &= ~GACT_RELVERIFY;

   /*
    * Font?
    */
   if (FindTagItem(BT_TextAttr, tstate) && md->md_TabsFont)
   {
      for (i = 0; i <= md->md_MaxLabel; i++)
         DoSetMethodNG(md->md_Objects[i], BT_TextAttr, md->md_TabsFont, TAG_END);
   }

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case GA_ID:
      case GA_Disabled:
      case BT_HelpFile:
      case BT_HelpNode:
      case BT_HelpLine:
      case BT_HelpText:
      case BT_HelpTextID:
      case BT_HelpHook:
         *ttag = *tag;
         for (i = 0; i <= md->md_MaxLabel; i++)
         {
            AsmDoMethodA(md->md_Objects[i], (Msg)&set);
         };
         break;

      case MX_DisableButton:
      case MX_EnableButton:
         if ((data >= 0) && (data <= md->md_MaxLabel))
            DoSetMethod(md->md_Objects[data], gi, GA_Disabled, (tag->ti_Tag == MX_DisableButton), TAG_DONE);
         break;

      case MX_Active:
         /*
          * Sanity check.
          */
         if ((int)data < 0)
         {
            if (md->md_Type) data = md->md_MaxLabel;
            else             data = 0;
         };
         if (data > md->md_MaxLabel)
         {
            if (md->md_Type) data = 0;
            else             data = md->md_MaxLabel;
         };
         if (data != md->md_ActiveLabel)
         {
            /*
             * De-select the previous active object if
             * it's different from the current active one.
             */
            DoSetMethod(md->md_Objects[md->md_ActiveLabel], gi, GA_Selected, FALSE, TAG_DONE);
            DoSetMethod(md->md_Objects[data],               gi, GA_Selected, TRUE,  TAG_DONE);
            DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, MX_Active, data, TAG_DONE);
            md->md_ActiveLabel = data;
         };
         break;
      };
   };
   return 1;
}
METHOD_END
///
/// OM_GET
/*
 * Get an attribute.
 */
METHOD(MXClassGet, struct opGet *, opg)
{
   MD       *md = INST_DATA(cl, obj);
   ULONG     rc = 1;

   switch (opg->opg_AttrID)
   {
   case MX_Active:
      *opg->opg_Storage = md->md_ActiveLabel;
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
/*
 * Dispose of the object.
 */
METHOD(MXClassDispose, Msg, msg)
{
   MD       *md = INST_DATA(cl, obj);

   if (md->md_Objects)
      BGUI_FreePoolMem(md->md_Objects);

   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * Key activation.
 */
METHOD(MXClassKeyActive, struct wmKeyInput *, wmki)
{
   MD       *md   = INST_DATA(cl, obj);
   UWORD     qual = wmki->wmki_IEvent->ie_Qualifier;
   int       num  = md->md_ActiveLabel;
   int       max  = md->md_MaxLabel;
   int       old, dir;

   /*
    * Shifted activation?
    */
   dir = (qual & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) ? -1 : 1;
   
   /*
    * Scan through the button list for the
    * next or previous enabled button.
    */
   old = num;
   do {
      num += dir;
      if (num < 0)   num = max;
      if (num > max) num = 0;
      if (!(GADGET(md->md_Objects[num])->Flags & GFLG_DISABLED)) break;
   }
   while (num != old);

   /*
    * Setup the new button.
    */
   if (num != md->md_ActiveLabel)
      DoSetMethod(obj, wmki->wmki_GInfo, MX_Active, num, TAG_END);

   /*
    * Return the ID.
    */
   *(wmki->wmki_ID) = GADGET(obj)->GadgetID;

   return WMKF_VERIFY;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,           (FUNCPTR)MXClassNew,
   OM_SET,           (FUNCPTR)MXClassSetUpdate,
   OM_UPDATE,        (FUNCPTR)MXClassSetUpdate,
   OM_GET,           (FUNCPTR)MXClassGet,
   OM_DISPOSE,       (FUNCPTR)MXClassDispose,
   WM_KEYACTIVE,     (FUNCPTR)MXClassKeyActive,
   DF_END,           NULL
};

/*
 * Initialize the mx class.
 */
makeproto Class *InitMxClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_GROUP_GADGET,
                         CLASS_ObjectSize,     sizeof(MD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
