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
 * Revision 42.3  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
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
 * Revision 42.0  2000/05/09 22:09:50  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:51  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.4  1999/08/30 04:57:39  mlemos
 * Made the methods that change group members on-fly setup the gadget
 * attributes using the window WINDOW_SETUPGADGET method.
 *
 * Revision 41.10.2.3  1999/08/30 00:22:54  mlemos
 * Made the BASE_INHIBIT method call the superclass to set the base object
 * inhibit flag.
 * Made an added, inserted or replaced page member object be inhibited if it
 * is not active or the page object is inhibited.
 * Adjusted  the  active  page  number  and  node pointer when removing a page
 * member.
 *
 * Revision 41.10.2.2  1999/08/29 18:58:49  mlemos
 * Added support to the LGO_Relayout attribute to be able to not relayout a
 * when calling GRM_ADDMEMBER, GRM_INSERTMEMBER, GRM_REPLACEMEMBER.
 *
 * Revision 41.10.2.1  1999/08/29 17:08:39  mlemos
 * Added the implementation of the methods GRM_ADDMEMBER, GRM_REMMEMBER,
 * GRM_INSERTMEMBER, GRM_REPLACEMEMBER.
 *
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
//STATIC ASM PM *GetMember(REG(a0) PML *l, REG(d0) ULONG mnum)
STATIC ASM REGFUNC2(PM *, GetMember,
	REGPARAM(A0, PML *, l),
	REGPARAM(D0, ULONG, mnum))
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
REGFUNC_END

/*
 * Add members.
 */
//STATIC ASM VOID AddMembers( REG(a0) Class *cl, REG(a1) Object *obj, REG(a2) struct TagItem *attr)
STATIC ASM REGFUNC3(VOID, AddMembers,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A1, Object *, obj),
	REGPARAM(A2, struct TagItem *, attr))
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
REGFUNC_END

/*
 * Set members active/deactive.
 */
//STATIC ASM VOID DoMembers(REG(a0) PD *pd)
STATIC ASM REGFUNC1(VOID, DoMembers,
	REGPARAM(A0, PD *, pd))
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
REGFUNC_END

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(PageClassNew, struct opSet *, ops)
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
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Set/update page attributes.
 */
METHOD(PageClassSetUpdate, struct opUpdate *, opu)
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
METHOD_END
///
/// BASE_RENDER
/*
 * Render the page.
 */
METHOD(PageClassRender, struct bmRender *, bmr)
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
METHOD_END
///
/// OM_GET
/*
 * They want to know something.
 */
METHOD(PageClassGet, struct opGet *, opg )
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
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of all objects added to the page-list
 * and then dispose of the page itself.
 */
METHOD(PageClassDispose, Msg, msg)
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
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want to know something about
 * our dimensions.
 */
METHOD(PageClassDimensions, struct bmDimensions *, bmd)
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
METHOD_END
///
/// GRM_WHICHOBJECT
/*
 * Get the object under the mouse.
 */
METHOD(PageClassWhichObject, struct grmWhichObject *, grwo)
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
METHOD_END
///
/// #?_FORWARD
/*
 * Forward a message to the active page.
 */
METHOD(PageClassForward, Msg, msg)
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
METHOD_END
///
/// #?_ALL
/*
 * Pass the message to all submembers.
 */
METHOD(PageClassAll, Msg, msg)
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
METHOD_END
///
/// BASE_INHIBIT
/*
 * Inhibit an entire page object.
 */
METHOD(PageClassInhibit, struct bmInhibit *, bmi)
{
   PD       *pd = INST_DATA(cl, obj);
   PM       *pm, *active = pd->pd_Active;
   BOOL      inhibit = bmi->bmi_Inhibit;

   if(!AsmDoSuperMethodA(cl, obj, (Msg)bmi))
      return(0);
   if (inhibit) pd->pd_Flags |= PDF_INHIBIT;
   else         pd->pd_Flags &= ~PDF_INHIBIT;

   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      AsmDoMethod(pm->pm_Object, BASE_INHIBIT, inhibit || (pm != active));
   };

   return 1;
}
METHOD_END
///
/// BASE_IS_MULTI
METHOD(PageClassIsMulti, Msg, msg)
{
   return TRUE;
}
METHOD_END
///

///
/// GRM_ADDMEMBER
/*
 * Add a member to the group.
 */
METHOD(PageClassAddMember, struct grmAddMember *, grma)
{
   PD *pd = INST_DATA(cl, obj);
   PM *pm;

   if(!(pm = (PM *)BGUI_AllocPoolMem(sizeof(PM))))
      return(FALSE);
   pm->pm_Object=grma->grma_Member;
   Forbid();
   AddTail((struct List *)&pd->pd_Members, (struct Node *)pm);
   Permit();

   if(BASE_DATA(obj)->bc_Window)
   {
      struct TagItem tags[2];

      tags[0].ti_Tag=BT_Inhibit;
      tags[0].ti_Data=TRUE;
      tags[1].ti_Tag=TAG_END;
      AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET,pm->pm_Object,&tags);
   }

   /*
    * Try to re-layout the group.
    */

   if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grma->grma_Attr)
   && !RelayoutGroup(obj))
   {
      Forbid();
      Remove((struct Node *)pm);
      Permit();
      BGUI_FreePoolMem(pm);
      RelayoutGroup(obj);

      return FALSE;
   };
   return TRUE;
}
METHOD_END

///
/// GRM_REMMEMBER
/*
 * Remove an object from the list.
 */
METHOD(PageClassRemMember, struct grmRemMember *, grmr)
{
   PD *pd = INST_DATA(cl, obj);
   PM *pm;

   if(!grmr->grmr_Member)
      return(NULL);
   Forbid();
   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      if(pm->pm_Object==grmr->grmr_Member)
      {
         if(pm==pd->pd_Active)
            pd->pd_Active=(pm->pm_Prev->pm_Prev ? pm->pm_Prev : pm->pm_Next);
         Remove((struct Node *)pm);
         BGUI_FreePoolMem(pm);
         for(pd->pd_Num=0,pm=pd->pd_Members.pl_First;pm->pm_Next && pm!=pd->pd_Active;pd->pd_Num++,pm=pm->pm_Next);
         Permit();
         RelayoutGroup(obj);
         return((ULONG)grmr->grmr_Member);
      }
   }
   Permit();
   return(NULL);
}
METHOD_END

///
/// GRM_INSERTMEMBER
/*
 * Insert a member in the group.
 */
METHOD(PageClassInsert, struct grmInsertMember *, grmi)
{
   PD *pd = INST_DATA(cl, obj);
   PM *pm,*new_pm;

   if(!grmi->grmi_Member
   || !(new_pm = (PM *)BGUI_AllocPoolMem(sizeof(PM))))
      return(FALSE);
   new_pm->pm_Object=grmi->grmi_Member;
   Forbid();
   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      if(pm->pm_Object==grmi->grmi_Pred)
      {
         Insert((struct List *)&pd->pd_Members,(struct Node *)new_pm,(struct Node *)pm);
         Permit();

         if(BASE_DATA(obj)->bc_Window)
         {
            struct TagItem tags[2];

            tags[0].ti_Tag=BT_Inhibit;
            tags[0].ti_Data=TRUE;
            tags[1].ti_Tag=TAG_END;
            AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET,pm->pm_Object,&tags);
         }

         /*
          * Try to re-layout the group.
          */
         if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grmi->grmi_Attr)
         && !RelayoutGroup(obj))
         {
            Forbid();
            Remove((struct Node *)new_pm);
            Permit();
            BGUI_FreePoolMem(new_pm);
            RelayoutGroup(obj);
         
            return FALSE;
         };
         return TRUE;
      }
   }
   Permit();
   BGUI_FreePoolMem(new_pm);
   return(FALSE);
}
METHOD_END

///
/// GRM_REPLACEMEMBER
/*
 * Replace a member in the group.
 */
METHOD(PageClassReplace, struct grmReplaceMember *, grrm)
{
   PD *pd = INST_DATA(cl, obj);
   PM *pm;

   /*
    * No NULL-objects.
    */
   if(!grrm->grrm_MemberA
   || !grrm->grrm_MemberB)
      return(FALSE);
   Forbid();
   for (pm = pd->pd_Members.pl_First; pm->pm_Next; pm = pm->pm_Next)
   {
      if(pm->pm_Object==grrm->grrm_MemberA)
      {
         pm->pm_Object=grrm->grrm_MemberB;
         Permit();

         if(BASE_DATA(obj)->bc_Window)
         {
            struct TagItem tags[2];

            tags[0].ti_Tag=BT_Inhibit;
            tags[0].ti_Data=((pd->pd_Flags & PDF_INHIBIT)!=0 || (pm != pd->pd_Active));
            tags[1].ti_Tag=TAG_END;
            AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET,pm->pm_Object,&tags);
         }

         /*
          * Try to re-layout the group.
          */
         if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grrm->grrm_Attr)
         && !RelayoutGroup(obj))
         {
            Forbid();
            pm->pm_Object=grrm->grrm_MemberA;
            Permit();
            RelayoutGroup(obj);
         
            return FALSE;
         };
         return((ULONG)grrm->grrm_MemberA);
      }
   }
   Permit();
   return 0;
}
METHOD_END

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
   GRM_ADDMEMBER,          (FUNCPTR)PageClassAddMember,
   GRM_REMMEMBER,          (FUNCPTR)PageClassRemMember,
   GRM_INSERTMEMBER,       (FUNCPTR)PageClassInsert,
   GRM_REPLACEMEMBER,      (FUNCPTR)PageClassReplace,
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
