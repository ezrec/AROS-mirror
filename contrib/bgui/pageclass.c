/*
 * @(#) $Header$
 *
 * BGUI library
 * pageclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:12:47  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:20  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions.

#include "include/classdefs.h"

/*
 * Page members are tied together
 * using these structures.
 */
typedef struct PMember {
   struct PMember *pm_Next;      /* next member                 */
   struct PMember *pm_Prev;      /* previous member             */
   Object         *pm_Object;    /* the member                  */
}  PM;

typedef struct {
   PM             *pl_First;     /* first memeber               */
   PM             *pl_EndMark;   /* end-of-list marker          */
   PM             *pl_Last;      /* last member                 */
}  PML;

/*
 * Object instance data.
 */
typedef struct {
   ULONG           pd_Flags;     /* see below                   */
   PML             pd_Members;   /* list of members             */
   PM             *pd_Active;    /* points to the active member */
   ULONG           pd_Num;       /* member number.              */
}  PD;

#define PDF_ADDFAILURE     (1<<0)            /* create time failure        */
#define PDF_INVERTED       (1<<1)            /* AddHead()                  */
#define PDF_INHIBIT        (1<<2)            /* Inhibited?                 */

///

/*
 * Get the member 'num'.
 */
STATIC ASM PM *GetMember(REG(a0) PML *l, REG(d0) ULONG mnum)
{
   PM          *pm;
   ULONG        num = 0;

   for (pm = l->pl_First; pm->pm_Next; pm = pm->pm_Next, num++)
   {
      if (num == mnum)
         return pm;
   }
   return NULL;
}

/*
 * Add members.
 */
STATIC ASM VOID AddMembers( REG(a0) Class *cl, REG(a1) Object *obj, REG(a2) struct TagItem *attr)
{
   PD                *pd = INST_DATA(cl, obj);
   PM                *pm;
   struct TagItem    *tstate = attr, *tag;

   /*
    * Scan attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      switch (tag->ti_Tag)
      {
      case PAGE_Member:
         /*
          * We do not allow NULL-objects.
          */
         if (tag->ti_Data)
         {
            /*
             * Allocate PMember structure.
             */
            if (pm = (PM *)BGUI_AllocPoolMem(sizeof(PM)))
            {
               /*
                * Initialize structure and add it to the list.
                */
               pm->pm_Object = (Object *)tag->ti_Data;
               if (pd->pd_Flags & PDF_INVERTED)
                  AddHead((struct List *)&pd->pd_Members, (struct Node *)pm);
               else
                  AddTail((struct List *)&pd->pd_Members, (struct Node *)pm);
            } else
               /*
                * Memory error.
                */
               pd->pd_Flags |= PDF_ADDFAILURE;
         } else
            /*
             * NULL-object.
             */
            pd->pd_Flags |= PDF_ADDFAILURE;
         break;
      };
   };
}

/*
 * Set members active/deactive.
 */
STATIC ASM VOID DoMembers(REG(a0) PD *pd)
{
   PM       *pm, *active = pd->pd_Active;
   BOOL      inhibit = pd->pd_Flags & PDF_INHIBIT;

   /*
    * Loop members.
    */
   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      /*
       * (De)activate members.
       */
      AsmDoMethod(pm->pm_Object, BASE_INHIBIT, inhibit || (pm != active));
   };
}

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(PageClassNew, struct opSet *ops)
{
   PD                *pd;
   struct TagItem    *tags, *tag;
   ULONG              rc;

   tags = DefTagList(BGUI_PAGE_GADGET, ops->ops_AttrList);

   /*
    * First we let the superclass
    * get us an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      pd = INST_DATA(cl, rc);

      if (tag = FindTagItem(PAGE_NoBufferRP, tags))
      {
         DoSetMethodNG((Object *)rc, BT_Buffer, !(tag->ti_Data), TAG_DONE);
      };

      /*
       * Initialize the page
       * member list.
       */
      NewList((struct List *)&pd->pd_Members);

      GADGET(rc)->Activation |= GACT_RELVERIFY;

      if (GetTagData(PAGE_Inverted, FALSE, tags))
         pd->pd_Flags |= PDF_INVERTED;

      /*
       * Add all page members.
       */
      AddMembers(cl, (Object *)rc, ops->ops_AttrList);

      pd->pd_Flags &= ~PDF_INVERTED;

      /*
       * When successfull return the object.
       */
      if (!(pd->pd_Flags & PDF_ADDFAILURE))
      {
         /*
          * Get the active member.
          */
         pd->pd_Num    = GetTagData(PAGE_Active, 0, tags);
         pd->pd_Active = GetMember(&pd->pd_Members, pd->pd_Num);
         DoMembers(pd);
      }
      else
      {
         /*
          * Otherwise dispose of the page.
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
///
/// OM_SET, OM_UPDATE
/*
 * Set/update page attributes.
 */
METHOD(PageClassSetUpdate, struct opUpdate *opu)
{
   PD                *pd = INST_DATA( cl, obj );
   struct TagItem    *attr = opu->opu_AttrList, *tstate = attr, *tag;
   struct GadgetInfo *gi = opu->opu_GInfo;
   PM                *pm;
   ULONG              data;
   BOOL               vc = FALSE;
   
   /*
    * First we let the superclass
    * do it's thing.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   GADGET(obj)->Activation |= GACT_RELVERIFY;

   /*
    * We do not respond
    * to interim messages.
    */
   if ((opu->MethodID == OM_UPDATE) && (opu->opu_Flags & OPUF_INTERIM))
      return 0;

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case GA_TopBorder:
      case GA_BottomBorder:
      case GA_LeftBorder:
      case GA_RightBorder:
      case BT_ParentWindow:
      case BT_ParentView:
      case BT_TextAttr:
      case FRM_ThinFrame:
         /*
          * Pass on attribute to the members.
          */
         for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
            DoSetMethod(pm->pm_Object, gi, tag->ti_Tag, data, TAG_DONE);
         break;

      case PAGE_Active:
         if (pd->pd_Num != data)
         {
            pd->pd_Num     = data;
            pd->pd_Active  = GetMember(&pd->pd_Members, data);
            DoMembers(pd);
            vc = TRUE;
         };
         break;
      };
   };

   /*
    * When a visual change is necessary we re-render.
    */
   if (vc)
   {
      DoRenderMethod(obj, gi, GREDRAW_REDRAW);     
      DoNotifyMethod(obj, gi, 0, PAGE_Active, pd->pd_Num, TAG_END);
   };
   return 1;
}
///
/// BASE_RENDER
/*
 * Render the page.
 */
METHOD(PageClassRender, struct bmRender *bmr)
{
   PD                *pd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct BaseInfo   *bi = bmr->bmr_BInfo;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Then we go.
    */
   if (pd->pd_Active)
   {
      /*
       * Setup the active member size.
       */
      DoSetMethodNG(pd->pd_Active->pm_Object, GA_Left,  bc->bc_InnerBox.Left,  GA_Top,    bc->bc_InnerBox.Top,
                                              GA_Width, bc->bc_InnerBox.Width, GA_Height, bc->bc_InnerBox.Height,
                                              BT_ParentGroup, obj, TAG_DONE);
      /*
       * Render it.
       */
      AsmDoMethod(pd->pd_Active->pm_Object, GM_RENDER, bi, bi->bi_RPort, GREDRAW_REDRAW);
   };
   return 1;
}
///
/// OM_GET
/*
 * They want to know something.
 */
METHOD(PageClassGet, struct opGet *opg )
{
   PD       *pd = INST_DATA(cl, obj);
   ULONG     rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case PAGE_Active:
      if (pd->pd_Active) STORE pd->pd_Num;
      else               STORE ~0;
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
///
/// OM_DISPOSE
/*
 * Dispose of all objects added to the page-list
 * and then dispose of the page itself.
 */
METHOD(PageClassDispose, Msg msg)
{
   PD       *pd = INST_DATA(cl, obj);
   PM       *pm;

   /*
    * Remove all objects from the list.
    */
   while (pm = (PM *)RemHead((struct List *)&pd->pd_Members))
   {
      /*
       * Tell the object to
       * dispose.
       */
      AsmDoMethod(pm->pm_Object, OM_DISPOSE);

      /*
       * Deallocate the object
       * it's PM structure.
       */
      BGUI_FreePoolMem(pm);
   };

   /*
    * Call the super class
    * to dispose the pageclass object.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
///
/// BASE_DIMENSIONS
/*
 * They want to know something about
 * our dimensions.
 */
METHOD(PageClassDimensions, struct bmDimensions *bmd)
{
   PD              *pd = INST_DATA(cl, obj);
   struct BaseInfo *bi = bmd->bmd_BInfo;
   PM              *pm;
   UWORD            mw = 0, w;
   UWORD            mh = 0, h;

   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      w = h = 0;
      AsmDoMethod(pm->pm_Object, GRM_DIMENSIONS, bi, bi->bi_RPort, &w, &h, 0);
      if (w > mw) mw = w;
      if (h > mh) mh = h;
   }
   return CalcDimensions(cl, obj, bmd, mw, mh);
}
///
/// GRM_WHICHOBJECT
/*
 * Get the object under the mouse.
 */
METHOD(PageClassWhichObject, struct grmWhichObject *grwo)
{
   PD       *pd = INST_DATA(cl, obj);
   Object   *ob = pd->pd_Active->pm_Object;
   /*
    * Current page member a group?
    */
   if (IsMulti(ob))
      /*
       * Route message.
       */
      ob = (Object *)AsmDoMethodA(ob, (Msg)grwo);

   return (ULONG)ob;
}
///
/// #?_FORWARD
/*
 * Forward a message to the active page.
 */
METHOD(PageClassForward, Msg msg)
{
   PD       *pd = INST_DATA(cl, obj);
   ULONG     rc = GMR_NOREUSE;

   /*
    * Do we have an active member?
    */
   if (pd->pd_Active)
   {
      /*
       * Forward the message to the
       * active member.
       */
      rc = AsmDoMethodA(pd->pd_Active->pm_Object, msg);

      /*
       * Take over the active member's GadgetID.
       */
      GADGET(obj)->GadgetID = GADGET(pd->pd_Active->pm_Object)->GadgetID;
   }
   return rc;
}
///
/// #?_ALL
/*
 * Pass the message to all submembers.
 */
METHOD(PageClassAll, Msg msg)
{
   PD       *pd = INST_DATA(cl, obj);
   PM       *pm;
   ULONG     rc = 0;

   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      rc += AsmDoMethodA(pm->pm_Object, msg);
   };
   return rc;
}
///
/// BASE_INHIBIT
/*
 * Inhibit an entire page object.
 */
METHOD(PageClassInhibit, struct bmInhibit *bmi)
{
   PD       *pd = INST_DATA(cl, obj);
   PM       *pm, *active = pd->pd_Active;
   BOOL      inhibit = bmi->bmi_Inhibit;

   if (inhibit) pd->pd_Flags |= PDF_INHIBIT;
   else         pd->pd_Flags &= ~PDF_INHIBIT;

   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      AsmDoMethod(pm->pm_Object, BASE_INHIBIT, inhibit || (pm != active));
   };

   return 1;
}
///
/// BASE_IS_MULTI
METHOD(PageClassIsMulti, Msg msg)
{
   return TRUE;
}
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,            (FUNCPTR)PageClassRender,
   BASE_DIMENSIONS,        (FUNCPTR)PageClassDimensions,
   OM_NEW,                 (FUNCPTR)PageClassNew,
   OM_SET,                 (FUNCPTR)PageClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)PageClassSetUpdate,
   OM_DISPOSE,             (FUNCPTR)PageClassDispose,
   OM_GET,                 (FUNCPTR)PageClassGet,
   GM_GOINACTIVE,          (FUNCPTR)PageClassForward,
   GM_GOACTIVE,            (FUNCPTR)PageClassForward,
   GM_HANDLEINPUT,         (FUNCPTR)PageClassForward,
   GM_HITTEST,             (FUNCPTR)PageClassForward,
   GRM_WHICHOBJECT,        (FUNCPTR)PageClassWhichObject,
   BASE_INHIBIT,           (FUNCPTR)PageClassInhibit,
   BASE_MOVEBOUNDS,        (FUNCPTR)PageClassForward,
   BASE_LOCALIZE,          (FUNCPTR)PageClassAll,
   BASE_KEYLABEL,          (FUNCPTR)PageClassAll,
   BASE_FINDKEY,           (FUNCPTR)PageClassForward,
   BASE_SHOWHELP,          (FUNCPTR)PageClassForward,
   BASE_IS_MULTI,          (FUNCPTR)PageClassIsMulti,
   DF_END,                 NULL
};

/*
 * Simple initialization of the page class.
 */
makeproto Class *InitPageClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(PD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
