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
 * Revision 42.5  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.4  2003/01/18 19:09:56  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/06/01 01:41:37  bergers
 * Only 2 linker problems left: stch_l & stcu_d. Somebody might want to replace them (embraced by #ifdef __AROS__), please.
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
 * Revision 42.0  2000/05/09 22:08:50  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:12  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1998/10/12 01:26:10  mlemos
 * Integrated Janne patch to ensure enough stack space when the methods are
 * called and to restructured the code to be more inline with the other
 * classes.
 * Ensured that BASE_LAYOUT method calls are not forwarded to the external
 * object.
 *
 * Revision 41.10  1998/02/25 21:11:58  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:07  mlemos
 * Ian sources
 *
 *
 */

/// Class definitions
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
#ifdef __AROS__
/* looks like proto isn't needed, because ExtClassDispatch is defined
   before it is used (or better: accessed) the first time */
#else
SAVEDS ASM STATIC ULONG ExtClassDispatch( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );
#endif

///


/// SetupAttrList()
/*
 * Build the create tags and tracked tags.
 */
//STATIC ASM BOOL SetupAttrList(REG(a0) ED *ed, REG(a1) struct TagItem *attr)
STATIC ASM REGFUNC2(BOOL, SetupAttrList,
	REGPARAM(A0, ED *, ed),
	REGPARAM(A1, struct TagItem *, attr))
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
REGFUNC_END
///
///GetTrackChanges()
/*
 * Set the track changes.
 */
//STATIC ASM VOID GetTrackChanges( REG(a0) ED *ed )
STATIC ASM REGFUNC1(VOID, GetTrackChanges,
	REGPARAM(A0, ED *, ed))
{
   struct TagItem    *tstate = ed->ed_TrackList, *tag;

   /*
    * Get the attributes to track in the new object.
    */
   while (tag = NextTagItem(&tstate))
      AsmDoMethod(ed->ed_Object, OM_GET, tag->ti_Tag, &tag->ti_Data);

}
REGFUNC_END
///
///SetupSize()
/*
 * Setup object size.
 */
//STATIC ASM VOID SetupSize(REG(a0) Class *cl, REG(a1) Object *obj)
STATIC ASM REGFUNC2(VOID, SetupSize,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A1, Object *, obj))
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
REGFUNC_END
///

/// OM_NEW
METHOD(ExtClassNew, struct opSet *, ops)
{
    ED              *ed;
    ULONG           rc;
    struct TagItem  *attr;

    if( rc = AsmDoSuperMethodA(cl,obj,(Msg)ops) )
    {
        /*
         * Get to the instance data.
         */
        ed = INST_DATA(cl, rc);

        /*
         * Get tag pointer.
         */
        attr = ops->ops_AttrList;

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
    return rc;
}
METHOD_END
///
/// OM_DISPOSE
METHOD(ExtClassDispose, Msg, msg)
{
    ED              *ed = INST_DATA(cl,obj);
    ULONG           rc;

    /*
     * Dispose the object.  We need to reset the ed_Object,
     * because otherwise the dispatcher will crash.  This is
     * due to the rootclass calling OM_REMOVE *after* the OM_DISPOSE -method
     * of this class has been executed.  I believe this cannot really
     * be prevented, due to the order which the method travels in the
     * class hierarchy. /JJ
     */
    if (ed->ed_Object) {
        AsmDoMethod(ed->ed_Object, OM_DISPOSE);
        ed->ed_Object = NULL;
    }

    /*
     * Free the attrlist and the tracked attributes.
     */
    if( ed->ed_AttrList)  FreeTagItems(ed->ed_AttrList);
    if( ed->ed_TrackList) FreeTagItems(ed->ed_TrackList);

    /*
     * The rest is done by the superclass...
     */
    rc = AsmDoSuperMethodA(cl, obj, msg);

    return rc;
}
METHOD_END
///
/// OM_GET
METHOD(ExtClassGet, struct opGet *, opg )
{
    ED              *ed = INST_DATA(cl,obj);
    ULONG           rc;

    /*
     * What do they want?
     */
    rc = 1;
    switch (opg->opg_AttrID)
    {
        case EXT_Object:
            *(opg->opg_Storage) = (ULONG)ed->ed_Object;
            break;

        default:
            rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);

            if (ed->ed_Object)
                rc = AsmDoMethodA(ed->ed_Object, (Msg)opg);

            break;
    }

    return rc;
}
METHOD_END
///
///BASE_DIMENSIONS
METHOD(ExtClassDimensions, struct bmDimensions *, msg)
{
    ED              *ed = INST_DATA(cl,obj);
    ULONG           rc;

    /*
     * Setup minimum size.
     */
    rc = CalcDimensions(cl, obj, msg, ed->ed_MinWidth, ed->ed_MinHeight);

    return rc;
}
METHOD_END
///
///GM_HITTEST
METHOD(ExtClassHitTest, Msg, msg)
{
    ED              *ed = INST_DATA(cl,obj);
    ULONG           rc;

    rc = AsmDoSuperMethodA(cl, obj, msg);
    /*
     * Are we hit?
     */
    if (rc == GMR_GADGETHIT) {
        /*
         * Forward message.
         */
        if (ed->ed_Object) rc = ForwardMsg(obj, ed->ed_Object, msg);
    }

    return rc;
}
METHOD_END
///
///GM_HANDLEINPUT,GM_GOACTIVE
METHOD(ExtClassHandleInput, Msg, msg)
{
    ED              *ed = INST_DATA(cl,obj);
    ULONG           rc = GMR_NOREUSE;

    /*
     * Forward message.  The message is thrown away if there's no object.
     */
    if (ed->ed_Object) rc = ForwardMsg(obj, ed->ed_Object, msg);

    return rc;
}
METHOD_END
///
/// BASE_RENDER
METHOD(ExtClassRender, struct bmRender *, bmr)
{
    ED              *ed = INST_DATA(cl,obj);
    BC              *bc;
    ULONG           rc;
    BOOL            set;
    struct BaseInfo *bi;

    /*
     * Render the baseclass.
     */
    rc = AsmDoSuperMethodA(cl, obj, (Msg)bmr);

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

        bi = bmr->bmr_BInfo;
        rc = AsmDoMethod(ed->ed_Object, GM_RENDER, bi, bi->bi_RPort, bmr->bmr_Flags);
    }
    else
    {
        DisplayBeep(NULL);
        rc = 0;
    };

    return rc;
}
METHOD_END
///

///ExtClassDispatch()

/*
 * Class dispatcher.
 */
//SAVEDS ASM STATIC ULONG ExtClassDispatch( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
SAVEDS ASM STATIC REGFUNC3(ULONG, ExtClassDispatch,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   ED                *ed;
   ULONG              rc;
   APTR               stk;

#ifdef __AROS__
#warning Commented EnsureStack
#else
   stk = EnsureStack();
#endif
   /*
    * What have we got here...
    */
   switch (msg->MethodID)
   {
   case OM_NEW:
      rc = ExtClassNew( cl, obj, (struct opSet *)msg );
      break;

   case OM_DISPOSE:
      rc = ExtClassDispose( cl, obj, msg );
      break;

   case GM_RENDER:
   case BASE_LAYOUT:
      /*
       * We ignore it for now.
       */
      rc = AsmDoSuperMethodA(cl, obj, msg);
      break;

   case BASE_RENDER:
      rc = ExtClassRender(cl,obj, (struct bmRender *)msg);
      break;

   case OM_GET:
      rc = ExtClassGet(cl,obj, (struct opGet *)msg);
      break;

   case GM_HITTEST:
      rc = ExtClassHitTest(cl,obj,msg);
      break;

   case GM_GOACTIVE:
   case GM_HANDLEINPUT:
      rc = ExtClassHandleInput(cl,obj,msg);
      break;

   case BASE_DIMENSIONS:
      rc = ExtClassDimensions(cl,obj, (struct bmDimensions *)msg);
      break;

   default:
      /*
       * First we go.
       */
      rc = AsmDoSuperMethodA(cl, obj, msg);

      /*
       * Pass the message to the object.
       */
      ed = INST_DATA(cl,obj);

      if (ed->ed_Object)
         rc = AsmDoMethodA(ed->ed_Object, msg);

      break;
   }

#ifdef __AROS__
#else
   RevertStack(stk);
#endif
   return rc;
}
REGFUNC_END

///

/// Class initialization

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
///
