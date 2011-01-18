/*
 * @(#) $Header$
 *
 * BGUI library
 * groupclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.4  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2000/08/10 17:52:47  stegerg
 * temp fix for relayout refresh bug which only happens in AROS. temp. solved
 * by doing a RefreshGList in windowclass.c/WindowClassRelease method.
 *
 * Revision 42.2  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:47  stegerg
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
 * Revision 42.0  2000/05/09 22:09:07  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:22  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.11  1999/08/30 04:57:41  mlemos
 * Made the methods that change group members on-fly setup the gadget
 * attributes using the window WINDOW_SETUPGADGET method.
 *
 * Revision 41.10.2.10  1999/08/29 20:21:59  mlemos
 * Renamed the GRM_RELAYOUT to BASE_RELAYOUT.
 *
 * Revision 41.10.2.9  1999/08/29 18:58:51  mlemos
 * Added support to the LGO_Relayout attribute to be able to not relayout a
 * when calling GRM_ADDMEMBER, GRM_INSERTMEMBER, GRM_REPLACEMEMBER.
 *
 * Revision 41.10.2.8  1998/10/14 04:07:55  mlemos
 * Fixed the computation of group members minimum, maximum and nominam sizes.
 *
 * Revision 41.10.2.7  1998/07/05 19:19:26  mlemos
 * Fixed bug of Group node instance data pointer be passed instead of the
 * object pointer.
 *
 * Revision 41.10.2.6  1998/03/02 01:31:21  mlemos
 * Fixed members list memory leak when group has no members.
 * Added functions to obtain and release the members list.
 *
 * Revision 41.10.2.5  1998/03/02 00:22:18  mlemos
 * Fixed extention determination when a group is empty.
 *
 * Revision 41.10.2.4  1998/03/01 17:36:48  mlemos
 * Fixed memory leaks from group member list allocations.
 * Prevented nest calls to retrieve copies of the group members list.
 *
 * Revision 41.10.2.3  1998/02/28 02:44:06  mlemos
 * Made GRM_DIMENSIONS method obtain the group members list.
 *
 * Revision 41.10.2.2  1998/02/26 22:37:53  mlemos
 * Commented out unused variables.
 * Added casts to GadgetInfo structure where BaseInfo is passed.
 * Fixed Group Node Class RM_REMOVE call.
 *
 * Revision 41.10.2.1  1998/02/26 17:45:40  mlemos
 * Added support for getting a group node object
 *
 * Revision 41.10  1998/02/25 21:12:11  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:29  mlemos
 * Ian sources
 *
 *
 */

#define WW(x)

#include "include/classdefs.h"

/// Class definitions.
/*
 * Group member node.
 */
typedef struct {
   Object         *md_Object;                   /* The object itself.        */
   struct IBox     md_Bounds;                   /* Bounds relative to group. */
   ULONG           md_Weight;                   /* The object weight.        */
   WORD            md_FixedWidth;               /* Fixed width.              */
   WORD            md_FixedHeight;              /* Fixed height.             */
   UWORD           md_AspectX, md_AspectY;      /* Fixed Aspect ratio.       */
   UWORD           md_MinWidth, md_MinHeight;   /* Minimum width and height. */
   UWORD           md_MaxWidth, md_MaxHeight;   /* Maximum width and height. */
   UWORD           md_NomWidth, md_NomHeight;   /* Nominal width and height. */
   UWORD           md_HBLeft;                   /* Hit-box leftedge.         */
   UWORD           md_ScaledSize;               /* Scaled size.              */
   UWORD           md_Flags;                    /* See below.                */
   UBYTE           md_HAlign, md_VAlign;        /* Alignment.                */
   Object         *md_Group;                    /* The group we are in.      */
}  MD;

#define MDF_FIXEDWIDTH     (1<<0)         /* Fix width.                           */
#define MDF_FIXEDHEIGHT    (1<<1)         /* Fix height.                          */
#define MDF_SPACING        (1<<2)         /* This is a spacing object.            */
#define MDF_NO_ALIGN       (1<<3)         /* Object must not be aligned.          */
#define MDF_RESERVED       (1<<4)         /* Reserved bit.                        */
#define MDF_NO_NO_ALIGN    (1<<5)         /* Force alignment.                     */
#define MDF_HIDDEN         (1<<6)         /* Member hidden?                       */
///

/// SetGroupNodeAttrs

METHOD(SetGroupNodeAttrs, struct opSet *, ops)
{
   MD              *md = INST_DATA(cl, obj);
   struct TagItem  *tstate = ops->ops_AttrList, *tag;
   ULONG            data;
   BOOL             relayout = FALSE;

   /*
    * Scan LGO attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case LGO_Left:
         md->md_Bounds.Left = data;
         break;

      case LGO_Top:
         md->md_Bounds.Top = data;
         break;

      case LGO_Width:
         md->md_Bounds.Width = data;
         break;

      case LGO_Height:
         md->md_Bounds.Height = data;
         break;

      case LGO_Object:
         md->md_Object = (Object *)data;
         break;

      case LGO_Group:
         md->md_Group = (Object *)data;
         break;

      case LGO_SpaceObject:
         md->md_Object = BGUI_NewObjectA(BGUI_SPACING_OBJECT, NULL);
         md->md_Weight = data;
         md->md_Flags |= MDF_SPACING;
         break;

      case LGO_Weight:
         if (data < 1) data = 1;
         if (md->md_Weight != data)
         {
            md->md_Weight = data;
            relayout = TRUE;
         };
         break;

      case LGO_FixWidth:
         md->md_FixedWidth  = (WORD)data;
         break;

      case LGO_FixHeight:
         md->md_FixedHeight = (WORD)data;
         break;

      case LGO_FixMinWidth:
         md->md_FixedWidth  = data ? 0 : -1;
         break;

      case LGO_FixMinHeight:
         md->md_FixedHeight = data ? 0 : -1;
         break;

      case LGO_Align:
         if (data)
            md->md_Flags |= MDF_NO_NO_ALIGN;
         break;

      case LGO_NoAlign:
         if (data)
            md->md_Flags |= MDF_NO_ALIGN;
         break;

      case LGO_FixAspect:
         if (*(ULONG *)&md->md_AspectX != data)
         {
            *(ULONG *)&md->md_AspectX = data;
            relayout = TRUE;
         };
         break;

      case LGO_Visible:
         if (data)
         {
            if (md->md_Flags & MDF_HIDDEN)
            {
               md->md_Flags &= ~MDF_HIDDEN;
               relayout = TRUE;
            };
         }
         else
         {
            if (!(md->md_Flags & MDF_HIDDEN))
            {
               md->md_Flags |= MDF_HIDDEN;
               relayout = TRUE;
            };
         };
         break;
      };
   };
   if (relayout && md->md_Group) RelayoutGroup(md->md_Group);

   return 1;
}
METHOD_END
///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(GroupNodeClassNew, struct opSet *, ops)
{
   MD          *md;
   ULONG        rc;

   /*
    * First we let the superclass
    * create the object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      md                 = INST_DATA(cl, rc);
      md->md_Weight      = DEFAULT_WEIGHT;
      md->md_FixedWidth  = -1;
      md->md_FixedHeight = -1;

      /*
       * Setup user attributes.
       */
      SetGroupNodeAttrs(cl, (Object *)rc, ops);

      if (md->md_Object && md->md_Group)
      {
         DoSetMethodNG(md->md_Object, BT_GroupMember,  rc,
                                      BT_ParentGroup,  md->md_Group,
                                      BT_ParentWindow, BASE_DATA(md->md_Group)->bc_Window,
                                      TAG_DONE);

         return rc;
      };

      AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
   }
   return NULL;
}
METHOD_END
///
/// OM_SET
/*
 * Change one or more of the object
 * it's attributes.
 */
METHOD(GroupNodeClassSet, struct opSet *, ops)
{
   AsmDoSuperMethodA(cl, obj, (Msg)ops);

   return SetGroupNodeAttrs(cl, obj, ops);
}
METHOD_END
///
/// OM_GET
/*
 * Give one of the attributes.
 */
METHOD(GroupNodeClassGet, struct opGet *, opg)
{
   MD             *md = INST_DATA(cl, obj);
   ULONG           rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case LGO_MinWidth:
      STORE md->md_MinWidth;
      break;
   case LGO_MinHeight:
      STORE md->md_MinHeight;
      break;
   case LGO_MaxWidth:
      STORE md->md_MaxWidth;
      break;
   case LGO_MaxHeight:
      STORE md->md_MaxHeight;
      break;
   case LGO_NomWidth:
      STORE md->md_NomWidth;
      break;
   case LGO_NomHeight:
      STORE md->md_NomHeight;
      break;
   case LGO_Object:
      STORE md->md_Object;
      break;
   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END
///
/// RM_REMOVE
/*
 * Remove node.
 */
METHOD(GroupNodeClassRemove, Msg, msg)
{
   MD          *md = INST_DATA(cl, obj);

   if (md->md_Object)
     DoSetMethodNG(md->md_Object, BT_ParentWindow, NULL, BT_ParentGroup, NULL, BT_GroupMember, NULL, TAG_DONE);
   
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFuncNode[] = {
   OM_NEW,              (FUNCPTR)GroupNodeClassNew,
   OM_SET,              (FUNCPTR)GroupNodeClassSet,
   OM_GET,              (FUNCPTR)GroupNodeClassGet,
   RM_REMOVE,           (FUNCPTR)GroupNodeClassRemove,
   DF_END,              NULL
};

static Class *GMClass = NULL;
#define MEMBER_DATA(m) ((MD *)INST_DATA(GMClass, ((Object *)m)))

/*
 * Simple class initialization.
 */
makeproto Class *InitGroupNodeClass(void)
{
   return GMClass = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ROOT_OBJECT,
                                   CLASS_ObjectSize,     sizeof(MD),
                                   CLASS_DFTable,        ClassFuncNode,
                                   TAG_DONE);
}
///


/// Class definitions.

typedef struct {
   UWORD           td_Min;
   UWORD           td_Nom;
   UWORD           td_Max;
   UWORD           td_Size;
   UWORD           td_Weight;
   UWORD           td_PreAlign;
   UWORD           td_PostAlign;
}  TD;

/*
 * Object instance data.
 */
typedef struct {
   struct SignalSemaphore   gd_Lock;         /* Semaphore for safety purposes. */
   Object                 **gd_MD;           /* Array of object pointers.      */
   struct MinList           gd_Members;      /* Group members.                 */
   UWORD                    gd_NumMembers;   /* Number of members.             */
   UWORD                    gd_Rows;         /* Rows in HGroup.                */
   UWORD                    gd_Columns;      /* Columns in VGroup.             */
   UBYTE                    gd_HAlign;       /* Horizontal alignment.          */
   UBYTE                    gd_VAlign;       /* Vertical alignment.            */
   UWORD                    gd_SpaceX;       /* Inner-object spacing.          */
   UWORD                    gd_SpaceY;       /* Inner-object spacing.          */
   TD                      *gd_RD, *gd_CD;   /* Row and column descriptors.    */
   Object                  *gd_Active;       /* Active member.                 */
   UBYTE                    gd_Flags;        /* See below.                     */
   UBYTE                    gd_Style;        /* Style.                         */
}  GD;

#define GDF_ADD_FAILURE         (1<<0)         /* Error flag.                          */
#define GDF_HAS_TITLE           (1<<1)         /* Group is framed with a title.        */
#define GDF_IS_MASTER           (1<<2)         /* This is the master group.            */
#define GDF_EQUAL_MINWIDTH      (1<<3)         /* All objects have the same min width. */
#define GDF_EQUAL_MINHEIGHT     (1<<4)         /* All objects have the same min height.*/
#define GDF_BUFFER              (1<<5)         /* Buffer rendering.                    */
///

/// NextGroupNode

//static ASM MD *NextGroupNode(REG(a0) GD *gd, REG(a1) Object **node, REG(d0) ULONG flags)
static ASM REGFUNC3(MD *, NextGroupNode,
	REGPARAM(A0, GD *, gd),
	REGPARAM(A1, Object **, node),
	REGPARAM(D0, ULONG, flags))
{
   Object    *next;
   MD        *md;

   do
   {
      if (*node) next = (Object *)AsmDoMethod(*node, RM_NEXT);
      else       next = ListHeadObject((struct List *)&gd->gd_Members);

      if (!(*node = next)) return NULL;

      md = MEMBER_DATA(next);

   }  while (md->md_Flags & flags);

   return md;
}
REGFUNC_END
///
/// GroupNode

//STATIC ASM Object *GroupNode(REG(a0) GD *gd, REG(d0) int i, REG(d1) int j)
STATIC ASM REGFUNC3(Object *, GroupNode,
	REGPARAM(A0, GD *, gd),
	REGPARAM(D0, int, i),
	REGPARAM(D1, int, j))
{
   switch (gd->gd_Style)
   {
   case GRSTYLE_HORIZONTAL:
      j = (i * gd->gd_Columns) + j;
      break;
   case GRSTYLE_VERTICAL:
      j = (j * gd->gd_Rows) + i;
      break;
   default:
      return NULL;
   };

   if (j < gd->gd_NumMembers) return gd->gd_MD[j];

   return NULL;
}
REGFUNC_END
///
/// VSpace, HSpace

static UWORD VSpace(UWORD space)
{
   switch (space)
   {
   case (UWORD)GRSPACE_NARROW:
      return (UWORD)GetTagData(GROUP_DefVSpaceNarrow, 2, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   case (UWORD)GRSPACE_NORMAL:
      return (UWORD)GetTagData(GROUP_DefVSpaceNormal, 4, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   case (UWORD)GRSPACE_WIDE:
      return (UWORD)GetTagData(GROUP_DefVSpaceWide,   8, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   };
   return space;
}
static UWORD HSpace(UWORD space)
{
   switch (space)
   {
   case (UWORD)GRSPACE_NARROW:
      return (UWORD)GetTagData(GROUP_DefHSpaceNarrow, 2, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   case (UWORD)GRSPACE_NORMAL:
      return (UWORD)GetTagData(GROUP_DefHSpaceNormal, 4, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   case (UWORD)GRSPACE_WIDE:
      return (UWORD)GetTagData(GROUP_DefHSpaceWide,   8, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
   };
   return space;
}
///
/// FindObNode

//STATIC ASM Object *FindObNode(REG(a0) GD *gd, REG(a1) Object *find)
STATIC ASM REGFUNC2(Object *, FindObNode,
	REGPARAM(A0, GD *, gd),
	REGPARAM(A1, Object *, find))
{
   Object         *m = NULL;
   MD             *md;

   if (find)
   {
      /*
       * Try to locate the object.
       */
      while (md = NextGroupNode(gd, &m, 0))
      {
         /*
          * Is this the one?
          */
         if (md->md_Object == find) return m;
      };
   };
   return NULL;
}
REGFUNC_END
///

/// GROUPM_NEWMEMBER

METHOD(GroupClassNewMember, struct opSet *, ops)
{
   return (ULONG)BGUI_NewObjectA(BGUI_GROUP_NODE, ops->ops_AttrList);
}
METHOD_END
///
/// NewGroupMember
/*
 * Allocate and initialize a member structure.
 */
STATIC Object *NewGroupMember(Object *obj, Object *member, struct TagItem *attrs)
{
   ULONG tags[6], *t = tags;

   *t++ = LGO_Group;       *t++ = (ULONG)obj;
   *t++ = LGO_Object;      *t++ = (ULONG)member;
   *t++ = TAG_MORE;        *t++ = (ULONG)attrs;

   return (Object *)AsmDoMethod(obj, GROUPM_NEWMEMBER, tags, NULL);
}
///
/// NewSpaceObject
/*
 * Add a spacing object.
 */
STATIC Object *NewSpaceObject(Object *obj, ULONG weight)
{
   ULONG tags[6], *t = tags;

   *t++ = LGO_Group;       *t++ = (ULONG)obj;
   *t++ = LGO_SpaceObject; *t++ = (ULONG)weight;
   *t++ = TAG_DONE;

   return (Object *)AsmDoMethod(obj, GROUPM_NEWMEMBER, tags, NULL);
}
///
/// GroupSetAttrs
/*
 * Pass an attribute on to the members.
 */
//STATIC ASM void PassAttr(REG(a0) GD *gd, REG(a1) struct TagItem *tag, REG(a2) struct GadgetInfo *gi)
STATIC ASM REGFUNC3(void, PassAttr,
	REGPARAM(A0, GD *, gd),
	REGPARAM(A1, struct TagItem *, tag),
	REGPARAM(A2, struct GadgetInfo *, gi))
{
   Object         *m;
   MD             *md;
   struct opSet    ops;
   struct TagItem  T[2];

   T[0].ti_Tag      = tag->ti_Tag;
   T[0].ti_Data     = tag->ti_Data;
   T[1].ti_Tag      = TAG_DONE;

   ops.MethodID     = OM_SET;
   ops.ops_AttrList = T;
   ops.ops_GInfo    = gi;

   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING))
      AsmDoMethodA(md->md_Object, (Msg)&ops);
}
REGFUNC_END

/*
 * Set one or more attributes.
 */
METHOD(GroupSetAttrs, struct opSet *, ops)
{
   GD                *gd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct TagItem    *tstate = ops->ops_AttrList, *tag;
   ULONG              data, attr;
   struct GadgetInfo *gi = ops->ops_GInfo;

   /*
    * Make sure we stay with the correct activation flags.
    */
   GADGET(obj)->Activation |= GACT_RELVERIFY;

   /*
    * Pass on some tags.
    */
   while (tag = BGUI_NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case FRM_Title:
         if (data) gd->gd_Flags |= GDF_HAS_TITLE;
         else      gd->gd_Flags &= ~GDF_HAS_TITLE;
         break;

      case FRM_Type:
         if (data == FRTYPE_DEFAULT)
         {
            data = GetTagData(FRM_DefaultType, FRTYPE_NEXT, BGUI_GetDefaultTags(BGUI_GROUP_GADGET));
            DoSetMethodNG(bc->bc_Frame, FRM_Type, data, TAG_DONE);
         };
         break;

      case GA_TopBorder:
      case GA_BottomBorder:
      case GA_LeftBorder:
      case GA_RightBorder:
      case BT_TextAttr:
      case BT_ParentWindow:
      case BT_ParentView:
      case GA_Disabled:
      case FRM_ThinFrame:
         PassAttr(gd, tag, gi);
         break;

      case GROUP_IsMaster:
         /*
          * Master group?
          */
         if (data) gd->gd_Flags |=  GDF_IS_MASTER;
         else      gd->gd_Flags &= ~GDF_IS_MASTER;
         break;

      case GROUP_Spacing:
         gd->gd_SpaceX = gd->gd_SpaceY = data;
         break;

      case GROUP_HorizSpacing:
         gd->gd_SpaceX = data;
         break;

      case GROUP_VertSpacing:
         gd->gd_SpaceY = data;
         break;

      case GROUP_Rows:
         gd->gd_Rows = data;
         break;

      case GROUP_Columns:
         gd->gd_Columns = data;
         break;

      case GROUP_Style:
         gd->gd_Style = data;
         break;

      case GROUP_HorizAlign:
         gd->gd_HAlign = data;
         break;

      case GROUP_VertAlign:
         gd->gd_VAlign = data;
         break;

      case GROUP_EqualWidth:
         if (data) gd->gd_Flags |=  GDF_EQUAL_MINWIDTH;
         else      gd->gd_Flags &= ~GDF_EQUAL_MINWIDTH;
         break;

      case GROUP_EqualHeight:
         if (data) gd->gd_Flags |=  GDF_EQUAL_MINHEIGHT;
         else      gd->gd_Flags &= ~GDF_EQUAL_MINHEIGHT;
         break;

      case GROUP_BackFill:
         attr = FRM_BackFill;
         goto frame_attr;

      case GROUP_BackPen:
         attr = FRM_BackPen;
         goto frame_attr;

      case GROUP_BackDriPen:
         attr = FRM_BackDriPen;
         goto frame_attr;

      frame_attr:
         DoSuperSetMethodNG(cl, obj, bc->bc_Frame ? TAG_IGNORE : FRM_Type, FRTYPE_NONE, attr, data, TAG_DONE);
         break;
      };
   };
   return 1;
}
METHOD_END
///

/// OM_NEW
/*
 * Create a new object.
 */
METHOD(GroupClassNew, struct opSet *, ops)
{
   GD               *gd;
   struct TagItem   *tags = ops->ops_AttrList, *tstate, *tag;
   ULONG             rc;
   ULONG             data;
   BOOL              invert = FALSE;
   Object           *m;
   struct Node      *n1, *n2;
   int               l = 0, r = 0, t = 0, b = 0;

   /*
    * Let the superclass make an object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Obtain instance data.
       */
      gd = INST_DATA(cl, rc);

      /*
       * Initialize member list and other data.
       */
      NewList((struct List *)&gd->gd_Members);
      InitSemaphore(&gd->gd_Lock);
      gd->gd_MD=NULL;
      gd->gd_NumMembers=0;

      tstate = tags;
      /*
       * Initialize the rest of the instance data.
       */
      while (tag = BGUI_NextTagItem(&tstate))
      {
         data = tag->ti_Data;

         switch (tag->ti_Tag)
         {
         case GROUP_Offset:
            l = r = t = b = data;
            break;

         case GROUP_HorizOffset:
            l = r = data;
            break;

         case GROUP_VertOffset:
            t = b = data;
            break;

         case GROUP_LeftOffset:
            l = data;
            break;

         case GROUP_TopOffset:
            t = data;
            break;

         case GROUP_RightOffset:
            r = data;
            break;

         case GROUP_BottomOffset:
            b = data;
            break;

         case GROUP_Inverted:
            invert = data;
            break;

         case GROUP_SpaceObject:
            if (m = NewSpaceObject((Object *)rc, data))
            {
               AsmDoMethod(m, RM_ADDTAIL, &gd->gd_Members);
               break;
            };
            gd->gd_Flags |= GDF_ADD_FAILURE;
            break;

         case GROUP_Member:
            if (data)
            {
               if (m = NewGroupMember((Object *)rc, (Object *)data, tag + 1))
               {
                  AsmDoMethod(m, RM_ADDTAIL, &gd->gd_Members);
                  break;
               };
               /*
                * Dispose of the member.
                */
               AsmDoMethod((Object *)data, OM_DISPOSE);
            };
            /*
             * Set error flag.
             */
            gd->gd_Flags |= GDF_ADD_FAILURE;
            break;
         };
      };

      if (invert)
      {
         n1 = ((struct List *)&gd->gd_Members)->lh_Head;

         while ((n2 = n1->ln_Succ)->ln_Succ)
         {
            Remove(n2);
            AddHead((struct List *)&gd->gd_Members, n2);
         };
      };

      /*
       * All OK?
       */
      if (!(gd->gd_Flags & GDF_ADD_FAILURE))
      {
         if (GADGET(rc)->Activation & GACT_TOPBORDER) l++;

         DoSuperSetMethodNG(cl, (Object *)rc, BT_LeftOffset, HSpace(l),  BT_RightOffset,  HSpace(r),
                                              BT_TopOffset,  VSpace(t),  BT_BottomOffset, VSpace(b), TAG_DONE);

         GroupSetAttrs(cl, (Object *)rc, ops);
      }
      else
      {
         /*
          * Noop... dump the object.
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      };
   };
   return rc;
}
METHOD_END
///
/// OM_SET
/*
 * Set one or more attributes.
 */
METHOD(GroupClassSet, struct opSet *, ops)
{
   /*
    * First we let the superclass have a go at it.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)ops);

   return GroupSetAttrs(cl, obj, ops);
}
METHOD_END
///
/// OM_GET
METHOD(GroupClassGet, struct opGet *, opg)
{
   GD       *gd = INST_DATA(cl, obj);
   ULONG     rc = 1, *store = opg->opg_Storage;

   switch (opg->opg_AttrID)
   {
   case GROUP_Rows:
      STORE gd->gd_Rows;
      break;

   case GROUP_Columns:
      STORE gd->gd_Columns;
      break;

   case GROUP_HorizSpacing:
      STORE gd->gd_SpaceX;
      break;

   case GROUP_VertSpacing:
      STORE gd->gd_SpaceY;
      break;

   case GROUP_HorizAlign:
      STORE gd->gd_HAlign;
      break;

   case GROUP_VertAlign:
      STORE gd->gd_VAlign;
      break;

   case GROUP_Members:
      STORE gd->gd_MD;
      break;

   case GROUP_NumMembers:
      STORE gd->gd_NumMembers;
      break;

   case GROUP_Style:
      STORE gd->gd_Style;
      break;

   default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   };
   return rc;
}
METHOD_END

static Object **ObtainMembers(Object *obj,GD *gd,BOOL *got_members_list)
{
   if(gd->gd_MD==NULL)
   {
      if (!(gd->gd_MD = (Object **)AsmDoMethod(obj, GROUPM_OBTAINMEMBERS, GROMF_WAIT|GROMF_VISIBLE, &gd->gd_NumMembers)))
         return 0;
      *got_members_list=TRUE;
   }
   else
      *got_members_list=FALSE;
   return(gd->gd_MD);
}

static VOID ReleaseMembers(Object *obj,GD *gd,BOOL got_members_list)
{
   if(got_members_list)
   {
      AsmDoMethod(obj, GROUPM_RELEASEMEMBERS, gd->gd_MD);
      gd->gd_MD=NULL;
      gd->gd_NumMembers=0;
   }
}

///
/// BASE_LAYOUT
/*
 * Layout the group.
 */
METHOD(GroupClassLayout, struct bmLayout *, bml)
{
   GD           *gd = INST_DATA(cl, obj);
   BC           *bc = BASE_DATA(obj);
   Object       *m;
   MD           *md;
   TD           *td, *cd, *rd;
   BOOL          changed,got_members_list;
   ULONG         totalweight, totalsize, tw, th;
/*   UBYTE         align; */
   int           r, c, size, last;
   int           x, y, dx, dy, w, h;

   /*
    * Layout the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bml);

   if (!(bml->bml_Flags & BLF_CUSTOM_GROUP))
   {
      w = bc->bc_InnerBox.Width  - (HSpace(gd->gd_SpaceX) * (gd->gd_Columns - 1));
      h = bc->bc_InnerBox.Height - (VSpace(gd->gd_SpaceY) * (gd->gd_Rows - 1));

      if(ObtainMembers(obj,gd,&got_members_list)==NULL)
         return(0);
      if (!gd->gd_NumMembers)
      {
         ReleaseMembers(obj,gd,got_members_list);
         return 1;
      }

      /*
       * Set starting values.
       */
      tw = 0;
      for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
      {
         tw += (td->td_Size = td->td_Nom);
      };

      last = 0;
      do
      {
         /*
          * First loop to get total weight.
          */
         totalweight = 0;
         totalsize   = w;
         for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
         {
            if (td->td_Size < td->td_Max)
               totalweight += td->td_Weight;
            else
               totalsize   -= td->td_Size;
         };

         if ((totalweight == 0) || (totalsize <= 0)) break;

         /*
          * Loop to get all scaled sizes.
          */
         tw = 0;
         for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
         {
            if (td->td_Size < td->td_Max)
            {
               size = ScaleWeight(totalsize, totalweight, td->td_Weight);
               td->td_Size = range(size, td->td_Min, td->td_Max);
            };

            /*
             * Keep track of the total size.
             */
            tw += td->td_Size;
         };
         if (tw == last) break;
         last = tw;
      }  while (tw < w);

      /*
       * Constantly adjust the size of objects which where scaled larger
       * than their minimum size when the total scaled size was larger than
       * the area the objects must fit in.
       */
      while (tw > w)
      {
         changed = FALSE;
         for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
         {
            /*
             * Was the size bigger than the minimum size?
             */
            if (td->td_Size > td->td_Min)
            {
               changed = TRUE;
               td->td_Size--;
               tw--;
            };

            /*
             * Are we done yet?
             */
            if (tw <= w) break;
         };
         if (!changed) break;
      };

      /*
       * Set starting values.
       */
      th = 0;
      for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
      {
         th += (td->td_Size = td->td_Nom);
      };

      last = 0;
      do
      {
         /*
          * First loop to get total weight.
          */
         totalweight = 0;
         totalsize   = h;

         for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
         {
            if (td->td_Size < td->td_Max)
               totalweight += td->td_Weight;
            else
               totalsize   -= td->td_Size;
         };

         if ((totalweight == 0) || (totalsize <= 0)) break;

         /*
          * Loop to get all scaled sizes.
          */
         th = 0;
         for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
         {
            if (td->td_Size < td->td_Max)
            {
               size = ScaleWeight(totalsize, totalweight, td->td_Weight);
               td->td_Size = range(size, td->td_Min, td->td_Max);
            };

            /*
             * Keep track of the total size.
             */
            th += td->td_Size;
         };
         if (th == last) break;
         last = th;
      }  while (th < h);

      /*
       * Constantly adjust the size of objects which where scaled larger
       * than their minimum size when the total scaled size was larger than
       * the area the objects must fit in.
       */
      while (th > h)
      {
         changed = FALSE;
         for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
         {
            /*
             * Was the size bigger than the minimum size?
             */
            if (td->td_Size > td->td_Min)
            {
               changed = TRUE;
               td->td_Size--;
               th--;
            };

            /*
             * Are we done yet?
             */
            if (th <= h) break;
         };
         if (!changed) break;
      };

      /*
       * Scan through the members.
       */
      x = bc->bc_InnerBox.Left;

      for (c = 0, cd = gd->gd_CD; c < gd->gd_Columns; c++, cd++)
      {
         y = bc->bc_InnerBox.Top;

         for (r = 0, rd = gd->gd_RD; r < gd->gd_Rows; r++, rd++)
         {
            if (m = GroupNode(gd, r, c))
            {
               md = MEMBER_DATA(m);

               md->md_Bounds.Width  = range(cd->td_Size - md->md_HBLeft, md->md_MinWidth,  md->md_MaxWidth);
               md->md_Bounds.Height = range(rd->td_Size,                 md->md_MinHeight, md->md_MaxHeight);

               switch (md->md_HAlign ? md->md_HAlign : gd->gd_HAlign)
               {
               default:
               case GRALIGN_LEFT:
                  dx = md->md_HBLeft;
                  break;
               case GRALIGN_CENTER:
                  dx = max(0, cd->td_Size - md->md_Bounds.Width) >> 1;
                  break;
               case GRALIGN_RIGHT:
                  dx = max(0, cd->td_Size - md->md_Bounds.Width);
                  break;
               };

               switch (md->md_VAlign ? md->md_VAlign : gd->gd_VAlign)
               {
               default:
               case GRALIGN_TOP:
                  dy = 0;
                  break;
               case GRALIGN_CENTER:
                  dy = max(0, rd->td_Size - md->md_Bounds.Height) >> 1;
                  break;
               case GRALIGN_BOTTOM:
                  dy = max(0, rd->td_Size - md->md_Bounds.Height);
                  break;
               };

               md->md_Bounds.Left = x + dx;
               md->md_Bounds.Top  = y + dy;
            };
            y += rd->td_Size + VSpace(gd->gd_SpaceY);
         };
         x += cd->td_Size + HSpace(gd->gd_SpaceX);
      }
      ReleaseMembers(obj,gd,got_members_list);
   };

   return 1;
}
METHOD_END
///
/// BASE_RENDER

struct SubRender
{
   struct Message     sr_Message;
   Object            *sr_Object;
   struct GadgetInfo *sr_GInfo;
   struct RastPort   *sr_RPort;
};

VOID SAVEDS subrender_task(void)
{
   struct Process    *proc = (struct Process *)FindTask(NULL);
   struct SubRender  *sr;

   WaitPort(&proc->pr_MsgPort);
   sr = (struct SubRender *)GetMsg(&proc->pr_MsgPort);

   AsmDoMethod(sr->sr_Object, GM_RENDER, sr->sr_GInfo, sr->sr_RPort, GREDRAW_REDRAW);

   ReplyMsg((struct Message *)sr);
}

/*
 * Render the group.
 */
METHOD(GroupClassRender, struct bmRender *, bmr)
{
   GD                *gd = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   Object            *m;
   MD                *md;
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   struct RastPort   *rp = bi->bi_RPort;
   struct Process    *proc;
   struct MsgPort    *msgport = NULL;
   struct SubRender  *sr;
/*   ULONG              rc = 0; */
   BOOL               subrender = FALSE; // no multitasking yet
   int                render_count = 0;

   if (gd->gd_Flags & GDF_IS_MASTER)
   {
      if (!bc->bc_Frame)
      {
         BSetDPenA(bi, BACKGROUNDPEN);
         BBoxFillA(bi, &bc->bc_OuterBox);
      };
   };

WW(kprintf("***GroupClassRender*** obj = %x\n", obj));

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   /*
    * Top-border group hack...
    */
   if (GADGET(obj)->Activation & GACT_TOPBORDER)
   {
      BSetDPenA(bi, SHADOWPEN);
      Move(rp, bc->bc_OuterBox.Left, bc->bc_OuterBox.Top + 1);
      Draw(rp, bc->bc_OuterBox.Left, bc->bc_OuterBox.Top + bc->bc_OuterBox.Height - 1);
   };

   if (subrender)
   {
      msgport = CreateMsgPort();
      if (!msgport) subrender = FALSE;
   };

   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING))
   {
      /*
       * Setup object position/dimensions and render it.
       */
      SetGadgetBounds(md->md_Object, &md->md_Bounds);

      proc = NULL;

      if (subrender)
      {
         if (sr = BGUI_AllocPoolMem(sizeof(struct SubRender)))
         {
            proc = CreateNewProcTags(NP_Entry, subrender_task, NP_Name, "BGUI Render", TAG_DONE);

            if (!proc) BGUI_FreePoolMem(sr);
         };
      }

      if (proc)
      {
         render_count++;

         sr->sr_Message.mn_ReplyPort = msgport;
         sr->sr_Message.mn_Length    = sizeof(struct SubRender);
         sr->sr_Object               = md->md_Object;
         sr->sr_GInfo                = (struct GadgetInfo *)bi;
         sr->sr_RPort                = rp;

         PutMsg(&proc->pr_MsgPort, &sr->sr_Message);
      }
      else
      {
         AsmDoMethod(md->md_Object, GM_RENDER, bi, rp, GREDRAW_REDRAW);
      };
   };
   if (subrender)
   {
      /*
       * Wait for all subtasks to finish.
       */
      while (render_count)
      {
         WaitPort(msgport);
         while (sr = (struct SubRender *)GetMsg(msgport))
         {
            BGUI_FreePoolMem(sr);
            render_count--;
         };
      };
      DeleteMsgPort(msgport);
   };
   return 1;
}
METHOD_END
///
/// GM_HITTEST
/*
 * Pass on the hittest message.
 */
METHOD(GroupClassHitTest, struct gpHitTest *, gph)
{
   GD                *gd = INST_DATA(cl, obj);
   Object            *m;
   MD                *md;
   ULONG              rc;

   /*
    * See if a member was "hit".
    */
   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING))
   {
      /*
       * This member hit?
       */
      if ((rc = ForwardMsg(obj, md->md_Object, (Msg)gph)) == GMR_GADGETHIT)
      {
         /*
          * Make it the active one.
          */
         gd->gd_Active = md->md_Object;
         return rc;
      };
   };

   /*
    * Safety precautions.
    */
   gd->gd_Active = NULL;

   return 0;
}
METHOD_END
///
/// GM_GOACTIVE, GM_HANDLEINPUT
/*
 * Forward a GM_GOACTIVE or GM_HANDLEINPUT message.
 */
METHOD(GroupClassActiveInput, struct gpInput *, gpi)
{
   GD                *gd = INST_DATA(cl, obj);
   ULONG              rc = GMR_NOREUSE;

   /*
    * Do we have an active member?
    */
   if (gd->gd_Active)
   {
      /*
       * Tell'm its active.
       */
      if (gpi->MethodID == GM_GOACTIVE)
         DoSetMethodNG(gd->gd_Active, BT_IsActive, TRUE, TAG_END);

      rc = ForwardMsg(obj, gd->gd_Active, (Msg)gpi);

      /*
       * Clone the ID.
       */
      DoSuperSetMethodNG(cl, obj, GA_ID, GADGET(gd->gd_Active)->GadgetID, TAG_DONE);
   }
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE
/*
 * Forward the GM_GOINACTIVE message.
 */
METHOD(GroupClassInActive, Msg, msg)
{
   GD          *gd = INST_DATA(cl, obj);
   ULONG        rc = 0;

   /*
    * Inactivate the active member.
    */
   if (gd->gd_Active)
   {
      rc = AsmDoMethodA(gd->gd_Active, msg);
      /*
       * Tell 'm its in-active.
       */
      DoSetMethodNG(gd->gd_Active, BT_IsActive, FALSE, TAG_END);
      gd->gd_Active = NULL;
   };
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of the group and all
 * it's members.
 */
METHOD(GroupClassDispose, Msg, msg)
{
   GD           *gd = INST_DATA(cl, obj);
   Object       *m, *o;
   MD           *md;

   /*
    * Remove and dispose of all members.
    */
   while (m = ListHeadObject((struct List *)&gd->gd_Members))
   {
      md = MEMBER_DATA(m);
      o  = md->md_Object;

      /*
       * Free the node.
       */
      AsmDoMethod(m, OM_DISPOSE);

      /*
       * Dispose of the member.
       */
      AsmDoMethod(o, OM_DISPOSE);
   };

   if (gd->gd_RD) BGUI_FreePoolMem(gd->gd_RD);
   if (gd->gd_CD) BGUI_FreePoolMem(gd->gd_CD);

   /*
    * Leave the rest up to the superclass.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// GRM_ADDMEMBER
/*
 * Add a member to the group.
 */
METHOD(GroupClassAddMember, struct grmAddMember *, grma)
{
   GD             *gd = INST_DATA(cl, obj);
   Object         *m;

   /*
    * Allocate and initialize node.
    */
   if (m = NewGroupMember(obj, grma->grma_Member, (struct TagItem *)&grma->grma_Attr))
   {
      /*
       * Add it to the list.
       */
      AsmDoMethod(m, RM_ADDTAIL, &gd->gd_Members);

WW(kprintf("** GroupClassAddMember\n"));
WW(if(BASE_DATA(obj)->bc_Window) kprintf("** GroupClassAddMember: sending WM_SETUPGADGET to window\n"));

      if(BASE_DATA(obj)->bc_Window)
         AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET, grma->grma_Member, NULL);

      /*
       * Try to re-layout the group.
       */
WW(kprintf("** GroupClassAddMember: calling relayoutgroup: LGO_Relayout = %d\n",GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grma->grma_Attr)));

      if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grma->grma_Attr)
      && !RelayoutGroup(obj))
      {
WW(kprintf("** GroupClassAddMember: relayoutgroup failed. returning FALSE\n"));
         DisposeObject(m);
         RelayoutGroup(obj);

         return FALSE;
      };
WW(kprintf("** GroupClassAddMember: everything done. returning TRUE\n"));

      return TRUE;
   };
   return FALSE;
}
METHOD_END
///
/// GRM_INSERTMEMBER
/*
 * Insert a member in the group.
 */
METHOD(GroupClassInsert, struct grmInsertMember *, grmi)
{
   GD             *gd = INST_DATA(cl, obj);
   Object         *m;

   /*
    * No NULL-objects.
    */
   if (!grmi->grmi_Member)
      return FALSE;

   /*
    * Allocate and initialize node.
    */
   if (m = NewGroupMember(obj, grmi->grmi_Member, (struct TagItem *)&grmi->grmi_Attr))
   {
      AsmDoMethod(m, RM_INSERT, &gd->gd_Members, FindObNode(gd, grmi->grmi_Pred));

      if(BASE_DATA(obj)->bc_Window)
         AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET, grmi->grmi_Member, NULL);

      /*
       * Try to re-layout the group.
       */
      if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grmi->grmi_Attr)
      && !RelayoutGroup(obj))
      {
         DisposeObject(m);
         RelayoutGroup(obj);

         return FALSE;
      };
      return TRUE;
   };
   return FALSE;
}
METHOD_END
///
/// GRM_REPLACEMEMBER
/*
 * Replace a member in the group.
 */
METHOD(GroupClassReplace, struct grmReplaceMember *, grrm)
{
   GD             *gd = INST_DATA(cl, obj);
   Object         *m, *old;

   /*
    * No NULL-objects.
    */
   if (!grrm->grrm_MemberA || !grrm->grrm_MemberB)
      return 0;

   /*
    * Allocate and initialize node.
    */
   if (m = NewGroupMember(obj, grrm->grrm_MemberB, (struct TagItem *)&grrm->grrm_Attr))
   {
      /*
       * Find member to replace.
       */
      if (old = FindObNode(gd, grrm->grrm_MemberA))
      {
         AsmDoMethod(m, RM_INSERT, &gd->gd_Members, old);
         AsmDoMethod(old, RM_REMOVE);

         if(BASE_DATA(obj)->bc_Window)
            AsmDoMethod(BASE_DATA(obj)->bc_Window, WM_SETUPGADGET, grrm->grrm_MemberB, NULL);

         /*
          * Try to re-layout the group.
          */
         if(GetTagData(LGO_Relayout, TRUE, (struct TagItem *)&grrm->grrm_Attr)
         && !RelayoutGroup(obj))
         {
            AsmDoMethod(old, RM_INSERT, &gd->gd_Members);
            DisposeObject(m);
            RelayoutGroup(obj);

            return 0;
         };

         /*
          * Deallocate replaced node.
          */
         DisposeObject(old);

         /*
          * Set return code.
          */
         return (ULONG)grrm->grrm_MemberA;
      };
   };
   return 0;
}
METHOD_END
///
/// GRM_REMMEMBER
/*
 * Remove an object from the list.
 */
METHOD(GroupClassRemMember, struct grmRemMember *, grmr)
{
   GD             *gd = INST_DATA(cl, obj);
   Object         *m;

   if (m = FindObNode(gd, grmr->grmr_Member))
   {
      /*
       * Remove and dealocate it.
       */
      DisposeObject(m);

      /*
       * Relayout the group.
       */
      RelayoutGroup(obj);

      return (ULONG)grmr->grmr_Member;
   }
   return NULL;
}
METHOD_END
///
/// GROUPM_OBTAINMEMBERS

METHOD(GroupClassObtainMembers, struct gmObtainMembers *, gmom)
{
   GD      *gd = INST_DATA(cl, obj);
   MD      *md;
   Object **o, *m;
   int      n;
   ULONG    flags = gmom->gmom_Flags;

   if (flags & GROMF_WAIT)
   {
      if (flags & GROMF_EXCLUSIVE)
      {
         ObtainSemaphore(&gd->gd_Lock);
      }
      else
      {
         ObtainSemaphoreShared(&gd->gd_Lock);
      };
   }
   else
   {
      if (flags & GROMF_EXCLUSIVE)
      {
         n = AttemptSemaphore(&gd->gd_Lock);
      }
      else
      {
         n = AttemptSemaphoreShared(&gd->gd_Lock);
      };
      if (!n) return 0;
   };

   n = 0;
   m = NULL;
   while (md = NextGroupNode(gd, &m, (flags & GROMF_VISIBLE) ? MDF_HIDDEN : 0))
      n++;

   o = BGUI_AllocPoolMem(sizeof(Object *) * (n + 2));

   if (!o)
   {
      ReleaseSemaphore(&gd->gd_Lock);
      return 0;
   }

   *o++ = obj;  // Marker for safety.

   m = NULL;
   while (md = NextGroupNode(gd, &m, (flags & GROMF_VISIBLE) ? MDF_HIDDEN : 0))
   {
      if (flags & GROMF_OBJECTS)
         *o++ = md->md_Object;
      else
         *o++ = m;
   };

   if (gmom->gmom_Number) *(gmom->gmom_Number) = n;
   return (ULONG)(o - n);
}
METHOD_END
///
/// GROUPM_RELEASEMEMBERS

METHOD(GroupClassReleaseMembers, struct gmReleaseMembers *, gmrm)
{
   GD       *gd = INST_DATA(cl, obj);
   Object  **o = gmrm->gmrm_Array - 1;
   int       rc = -1;

   if (*o == obj)  // Check for safety marker.
   {
      if (gd->gd_Lock.ss_Owner == FindTask(NULL))
      {
         rc = gd->gd_Lock.ss_NestCount - 1;

         ReleaseSemaphore(&gd->gd_Lock);
      };
      BGUI_FreePoolMem(o);
   }
#ifdef DEBUG_BGUI
   else
      D(bug("*** Attempt to free an invalid group members list\n"));
#endif
   return (ULONG)rc;     // rc < 0 = failure, rc = 0 is freed, rc > 0 still locked.
}
METHOD_END
///
/// RelayoutGroup

//makeproto ASM ULONG RelayoutGroup(REG(a0) Object *obj)
makeproto ASM REGFUNC1(ULONG, RelayoutGroup,
	REGPARAM(A0, Object *, obj))
{
   BC                 *bc = BASE_DATA(obj);
   struct Window      *w = NULL;
   struct bmRelayout  bmr;
   ULONG               rc = 1;

WW(kprintf("** GroupClass_RelayoutGrop\n"));

   if (bc->bc_Window)
   {
WW(kprintf("** GroupClass_RelayoutGrop: has bc->bc_Window\n"));
      Get_Attr(bc->bc_Window, WINDOW_Window, (ULONG *)&w);

      if (w)
      {
         bmr.MethodID   = BASE_RELAYOUT;
         bmr.bmr_GInfo = NULL;
         bmr.bmr_RPort = w->RPort;
WW(kprintf("** GroupClass_RelayoutGrop: has WINDOW_Window --> sending BASE_RELAYOUT to obj\n"));

         rc = BGUI_DoGadgetMethodA(obj, w, NULL, (Msg)&bmr);
      };
   };
   return rc;
}
REGFUNC_END
///

/// MemberDimensions
/*
 * Calculate the minimum size of a group.
 */
METHOD(MemberDimensions, struct bmDimensions *, bmd)
{
   GD                *gd = INST_DATA(cl, obj);
   Object            *m;
   MD                *md;
   TD                *td;
   ULONG              minw, minh, nomw, nomh, maxw, maxh;
   ULONG              tmin, tmax, tnom;
   int                r, c;
   BOOL               got_members_list;

   if(ObtainMembers(obj,gd,&got_members_list)==NULL)
      return(0);
   if (gd->gd_NumMembers)
   {
      if (gd->gd_Style == GRSTYLE_HORIZONTAL)
      {
         gd->gd_Rows = range(gd->gd_Rows, 1, gd->gd_NumMembers);
         gd->gd_Columns = (gd->gd_NumMembers + (gd->gd_Rows - 1)) / gd->gd_Rows;
      };

      if (gd->gd_Style == GRSTYLE_VERTICAL)
      {
         gd->gd_Columns = range(gd->gd_Columns, 1, gd->gd_NumMembers);
         gd->gd_Rows = (gd->gd_NumMembers + (gd->gd_Columns - 1)) / gd->gd_Columns;
      };

      if (gd->gd_RD) BGUI_FreePoolMem(gd->gd_RD);
      if (gd->gd_CD) BGUI_FreePoolMem(gd->gd_CD);

      gd->gd_RD = BGUI_AllocPoolMem(sizeof(TD) * gd->gd_Rows);
      gd->gd_CD = BGUI_AllocPoolMem(sizeof(TD) * gd->gd_Columns);

      if (!(gd->gd_RD && gd->gd_CD)) return 0;

      /*
       * Scan through the members.
       */
      for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
      {
         td->td_PreAlign = 0;

         for (r = 0; r < gd->gd_Rows; r++)
         {
            if (m = GroupNode(gd, r, c))
            {
               md = MEMBER_DATA(m);

               /*
                * Find out how much we must adjust for alignment.
                */
               AsmDoMethod(md->md_Object, BASE_LEFTEXT, bmd->bmd_BInfo->bi_RPort, &md->md_HBLeft);

               if (!md->md_HBLeft)
               {
                  if (!(md->md_Flags & MDF_NO_NO_ALIGN))
                     md->md_Flags |= MDF_NO_ALIGN;
               }
               else
               {
                  if (md->md_HBLeft > td->td_PreAlign) td->td_PreAlign = md->md_HBLeft;
               };
            };
         };

         tmin = 0;
         tnom = 0;
         tmax = 0;

         for (r = 0; r < gd->gd_Rows; r++)
         {
            if (m = GroupNode(gd, r, c))
            {
               md = MEMBER_DATA(m);

               if (!(md->md_Flags & MDF_NO_ALIGN))
               {
                  md->md_HBLeft = td->td_PreAlign - md->md_HBLeft;
               }
               else
               {
                  md->md_HBLeft = 0;
               };

               if (r == 0) td->td_Weight = md->md_Weight;

               /*
                * Adjust the minimum width with the maximum extention.
                */
               tmin = max(tmin, md->md_MinWidth + md->md_HBLeft);
               tnom = max(tnom, md->md_NomWidth + md->md_HBLeft);
               tmax = max(tmax, md->md_MaxWidth);
            };
         };

         td->td_Min = tmin;
         td->td_Max = tmax;
         td->td_Nom = range(tnom, tmin, tmax);
      };

      /*
       * Scan through the members.
       */
      for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
      {
         tmin = 0;
         tnom = 0;
         tmax = 0;

         for (c = 0; c < gd->gd_Columns; c++)
         {
            if (m = GroupNode(gd, r, c))
            {
               md = MEMBER_DATA(m);

               if (c == 0) td->td_Weight = md->md_Weight;

               tmin = max(tmin, md->md_MinHeight);
               tnom = max(tnom, md->md_NomHeight);
               tmax = max(tmax, md->md_MaxHeight);
            };
         };

         td->td_Min = tmin;
         td->td_Max = tmax;
         td->td_Nom = range(tnom, tmin, tmax);
      };

      minw = nomw = HSpace(gd->gd_SpaceX) * (gd->gd_Columns - 1);
      minh = nomh = VSpace(gd->gd_SpaceY) * (gd->gd_Rows - 1);
      maxw = maxh = 0;

      for (r = 0, td = gd->gd_RD; r < gd->gd_Rows; r++, td++)
      {
         minh += td->td_Min;
         nomh += td->td_Nom;
         maxh += td->td_Max;
      };

      for (c = 0, td = gd->gd_CD; c < gd->gd_Columns; c++, td++)
      {
         minw += td->td_Min;
         nomw += td->td_Nom;
         maxw += td->td_Max;
      };
   }
   else
   {
      /*
       * No members.
       */
      minw = minh = 0;
      nomw = nomh = 0;
      maxw = maxh = 0xFFFF;
   };

   ReleaseMembers(obj,gd,got_members_list);

   minw = max(minw, 1);
   minh = max(minh, 1);

   maxw = range(maxw, minw, 0xFFFF);
   maxh = range(maxh, minh, 0xFFFF);

   nomw = range(nomw, minw, maxw);
   nomh = range(nomh, minh, maxh);

   bmd->bmd_Extent->be_Min.Width  += minw;
   bmd->bmd_Extent->be_Min.Height += minh;
   bmd->bmd_Extent->be_Nom.Width  += nomw;
   bmd->bmd_Extent->be_Nom.Height += nomh;
   bmd->bmd_Extent->be_Max.Width   = maxw;
   bmd->bmd_Extent->be_Max.Height  = maxh;

   return 1;
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want our dimensions info.
 */
METHOD(GroupClassDimensions, struct bmDimensions *, bmd)
{
   GD                   *gd = INST_DATA(cl, obj);
/*   BC                   *bc = BASE_DATA(obj); */
   Object               *m;
   MD                   *md;
   struct grmDimensions  dim;
   UWORD                 mw = 0, mh = 0, minw, minh, maxw, maxh, nomw, nomh;
   ULONG                 rc;

   dim.MethodID            = GRM_DIMENSIONS;
   dim.grmd_GInfo          = (struct GadgetInfo *)bmd->bmd_BInfo;
   dim.grmd_RPort          = bmd->bmd_BInfo->bi_RPort;
   dim.grmd_Flags          = bmd->bmd_Flags | GDIMF_MAXIMUM|GDIMF_NOMINAL;
   dim.grmd_MinSize.Width  = &minw;
   dim.grmd_MinSize.Height = &minh;
   dim.grmd_MaxSize.Width  = &maxw;
   dim.grmd_MaxSize.Height = &maxh;
   dim.grmd_NomSize.Width  = &nomw;
   dim.grmd_NomSize.Height = &nomh;

   /*
    * Browse through the members.
    */
   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN))
   {
      if (md->md_Flags & MDF_SPACING)
      {
         md->md_MinWidth  = 0;
         md->md_MinHeight = 0;
         md->md_NomWidth  = 0;
         md->md_NomHeight = 0;
         md->md_MaxWidth  = 0xFFFF;
         md->md_MaxHeight = 0xFFFF;
      }
      else
      {
         /*
          * Ask dimensions.
          */
         AsmDoMethodA(md->md_Object, (Msg)&dim);

         /*
          * Make sure the fixed sizes are not smaller than allowed.
          */
         if (md->md_FixedWidth >= 0)
         {
            maxw = minw = nomw = range(md->md_FixedWidth, minw, maxw);
         };
         if (md->md_FixedHeight >= 0)
         {
            maxh = minh = nomh = range(md->md_FixedHeight, minh, maxh);
         };
         if (minw > mw) mw = minw;
         if (minh > mh) mh = minh;

         md->md_MinWidth  = minw;
         md->md_MinHeight = minh;
         md->md_MaxWidth  = max(minw, maxw);
         md->md_MaxHeight = max(minh, maxh);
         md->md_NomWidth  = range(nomw, minw, maxw);
         md->md_NomHeight = range(nomh, minh, maxh);
      };
   };

   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN))
   {
      /*
       * Set equal width/height.
       */
      if (gd->gd_Flags & GDF_EQUAL_MINWIDTH)  md->md_MinWidth  = mw;
      if (gd->gd_Flags & GDF_EQUAL_MINHEIGHT) md->md_MinHeight = mh;
   };

   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)bmd))
   {
      /*
       * Calculate constraints.
       */
      if (!(bmd->bmd_Flags & BDF_CUSTOM_GROUP))
         rc = MemberDimensions(cl, obj, bmd);      
   };
   return rc;
}
METHOD_END
///
/// GRM_DIMENSIONS
METHOD(GroupClassDimensionsX, struct grmDimensions *, dim)
{
   GD    *gd = INST_DATA(cl, obj);
   ULONG rc=0;
   BOOL got_members_list;

   if(ObtainMembers(obj,gd,&got_members_list)==NULL)
      return(0);
   rc=AsmDoSuperMethodA(cl, obj, (Msg)dim);
   ReleaseMembers(obj,gd,got_members_list);
   return(rc);
}
METHOD_END

///
/// BASE_SHOWHELP
/*
 * Pass on the help message.
 */
METHOD(GroupClassHelp, struct bmShowHelp *, bsh)
{
   GD           *gd = INST_DATA(cl, obj);
   Object       *m;
   MD           *md;
   ULONG         rc = BMHELP_NOT_ME;

   m = NULL;
   while ((md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING)) && (rc == BMHELP_NOT_ME))
   {
      /*
       * Is this the one?
       */
      rc = AsmDoMethodA(md->md_Object, (Msg)bsh);
   };
   return rc;
}
METHOD_END
///
/// GRM_ADDSPACEMEMBER
/*
 * Add a spacing object.
 */
METHOD(GroupClassAddSpaceMember, struct grmAddSpaceMember *, grms)
{
   GD           *gd = INST_DATA(cl, obj);
   Object       *m;

   if (m = NewSpaceObject(obj, grms->grms_Weight))
   {
      AsmDoMethod(m, RM_ADDTAIL, &gd->gd_Members);
      return TRUE;
   };
   return FALSE;
}
METHOD_END
///
/// BASE_LEFTEXT
/*
 * Route a BASE_LEFTEXT method.
 */
METHOD(GroupClassLeftExt, struct bmLeftExt *, le)
{
   GD          *gd = INST_DATA(cl, obj);
   BC          *bc = BASE_DATA(obj);
   ULONG        rc;

   /*
    * Do we have a label or frame? If so we
    * take the left-extention of this object.
    */
   if (bc->bc_Label)
   {
      return AsmDoSuperMethodA(cl, obj, (Msg)le);
   };

   if (bc->bc_Frame)
   {
      Get_Attr(bc->bc_Frame, FRM_Type, &rc);
      if (rc != FRTYPE_NONE)
      {
         return AsmDoSuperMethodA(cl, obj, (Msg)le);
      };
   };

   /*
    * Get the biggest in the first column.
    */
   if(gd->gd_CD)
      *(le->bmle_Extention) = gd->gd_CD[0].td_PreAlign;
   else
   {
      *(le->bmle_Extention)=0;
   }

   return 1;
}
METHOD_END
///
/// GRM_WHICHOBJECT
/*
 * Which object is under the mouse?
 */
METHOD(GroupClassWhichObject, struct grmWhichObject *, grwo)
{
   GD              *gd = INST_DATA(cl, obj);
   struct IBox     *ibox;
   Object          *m;
   MD              *md;
   WORD             x, y;
   ULONG            rc;

   /*
    * Pick up the coords.
    */
   x = grwo->grwo_Coords.X;
   y = grwo->grwo_Coords.Y;

   /*
    * See if the coords are over a member.
    */
   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING))
   {
      /*
       * If this is a group/page we pass on the method.
       */
      if (IsMulti(md->md_Object))
      {
         if (rc = AsmDoMethodA(md->md_Object, (Msg)grwo))
            return rc;
      }

      /*
       * Get hitbox bounds.
       */
      ibox = GADGETBOX(md->md_Object);

      /*
       * Over this object?
       */
      if (x >= ibox->Left && x < (ibox->Left + ibox->Width ) &&
          y >= ibox->Top  && y < (ibox->Top  + ibox->Height))
         /*
          * Yes.
          */
         return (ULONG)md->md_Object;

   }
   return NULL;
}
METHOD_END
///
/// BASE_FINDKEY
/*
 * Which object has the proper key?
 */
METHOD(GroupClassFindKey, struct bmFindKey *, bmfk)
{
   GD           *gd = INST_DATA(cl, obj);
   Object       *m;
   MD           *md;
   ULONG         rc;
   
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)bmfk))
      return rc;
    
   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN | MDF_SPACING))
   {
      if (rc = AsmDoMethodA(md->md_Object, (Msg)bmfk))
         return rc;
   };
   return NULL;
}
METHOD_END
///
/// #?
/*
 * Attach keys from labels.
 */
METHOD(GroupClassAll, Msg, msg)
{
   GD           *gd = INST_DATA(cl, obj);
   Object       *m;
   MD           *md;
   ULONG         rc;
   
   rc = AsmDoSuperMethodA(cl, obj, msg);
   
   m = NULL;
   while (md = NextGroupNode(gd, &m, MDF_HIDDEN))
   {
      rc += AsmDoMethodA(md->md_Object, msg);
   };
   return rc;
}
METHOD_END
///
/// BASE_IS_MULTI
METHOD(GroupClassIsMulti, Msg, msg)
{
   return TRUE;
}
METHOD_END
///
/// Class initialization.
/*
 * Class function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,            (FUNCPTR)GroupClassRender,
   BASE_LAYOUT,            (FUNCPTR)GroupClassLayout,
   BASE_DIMENSIONS,        (FUNCPTR)GroupClassDimensions,
   GRM_DIMENSIONS,         (FUNCPTR)GroupClassDimensionsX,

   OM_NEW,                 (FUNCPTR)GroupClassNew,
   OM_SET,                 (FUNCPTR)GroupClassSet,
   OM_GET,                 (FUNCPTR)GroupClassGet,
   OM_DISPOSE,             (FUNCPTR)GroupClassDispose,
   GM_GOACTIVE,            (FUNCPTR)GroupClassActiveInput,
   GM_HANDLEINPUT,         (FUNCPTR)GroupClassActiveInput,
   GM_GOINACTIVE,          (FUNCPTR)GroupClassInActive,
   GM_HITTEST,             (FUNCPTR)GroupClassHitTest,

   GROUPM_OBTAINMEMBERS,   (FUNCPTR)GroupClassObtainMembers,
   GROUPM_RELEASEMEMBERS,  (FUNCPTR)GroupClassReleaseMembers,
   GROUPM_NEWMEMBER,       (FUNCPTR)GroupClassNewMember,
   GRM_ADDMEMBER,          (FUNCPTR)GroupClassAddMember,
   GRM_REMMEMBER,          (FUNCPTR)GroupClassRemMember,
   GRM_ADDSPACEMEMBER,     (FUNCPTR)GroupClassAddSpaceMember,
   GRM_INSERTMEMBER,       (FUNCPTR)GroupClassInsert,
   GRM_REPLACEMEMBER,      (FUNCPTR)GroupClassReplace,
   GRM_WHICHOBJECT,        (FUNCPTR)GroupClassWhichObject,
   BASE_MOVEBOUNDS,        (FUNCPTR)GroupClassAll,
   BASE_FINDKEY,           (FUNCPTR)GroupClassFindKey,
   BASE_KEYLABEL,          (FUNCPTR)GroupClassAll,
   BASE_LOCALIZE,          (FUNCPTR)GroupClassAll,
   BASE_SHOWHELP,          (FUNCPTR)GroupClassHelp,
   BASE_LEFTEXT,           (FUNCPTR)GroupClassLeftExt,
   BASE_INHIBIT,           (FUNCPTR)GroupClassAll,
   BASE_IS_MULTI,          (FUNCPTR)GroupClassIsMulti,

   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitGroupClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
                         CLASS_ObjectSize,     sizeof(GD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
