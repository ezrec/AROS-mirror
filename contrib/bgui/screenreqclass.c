/*
 * @(#) $Header$
 *
 * BGUI library
 * screenreqclass.c
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
 * Revision 42.0  2000/05/09 22:10:08  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:03  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:13:04  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:42  mlemos
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
   ULONG              sd_DisplayID;       /* Display mode ID                  */
   ULONG              sd_DisplayWidth;    /* Width of display in pixels       */
   ULONG              sd_DisplayHeight;   /* Height of display in pixels      */
   UWORD              sd_DisplayDepth;    /* Number of bit-planes of display  */
   UWORD              sd_OverscanType;    /* Type of overscan of display      */
   BOOL               sd_AutoScroll;      /* Display should auto-scroll?      */

   BOOL               sd_InfoOpen;        /* Info window opened on exit?      */
   WORD               sd_InfoLeft;        /* Last coordinates of Info window  */
   WORD               sd_InfoTop;
   WORD               sd_InfoWidth;
   WORD               sd_InfoHeight;
}  SD;

///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(ScreenReqClassNew, struct opSet *, ops)
{
   ULONG           rc;
   struct TagItem *tags;

   tags = DefTagList(BGUI_SCREENREQ_OBJECT, ops->ops_AttrList);

   /*
    * First we let the superclass create the object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Setup user attributes.
       */
      if (AsmCoerceMethod(cl, (Object *)rc, OM_SET, tags, NULL) != ASLREQ_OK)
      {
         /*
          * Failure.
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
/// OM_DISPOSE
/*
 * They want us gone.
 */
METHOD(ScreenReqClassDispose, Msg, msg)
{
   /*
    * The superclass does the rest.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Change one or more of the object
 * it's attributes.
 */
METHOD(ScreenReqClassSetUpdate, struct opSet *, ops)
{
   SD             *sd = INST_DATA(cl, obj);
   struct TagItem *tstate = ops->ops_AttrList, *tag;
   ULONG           data, rc;

   /*
    * First we let the superclass
    * have a go at it.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case ASLSM_InitialDisplayID:
      case SCREENREQ_DisplayID:
         sd->sd_DisplayID = data;
         break;
      case ASLSM_InitialDisplayWidth:
      case SCREENREQ_DisplayWidth:
         sd->sd_DisplayWidth = data;
         break;
      case ASLSM_InitialDisplayHeight:
      case SCREENREQ_DisplayHeight:
         sd->sd_DisplayHeight = data;
         break;
      case ASLSM_InitialDisplayDepth:
      case SCREENREQ_DisplayDepth:
         sd->sd_DisplayDepth = data;
         break;
      case ASLSM_InitialOverscanType:
      case SCREENREQ_OverscanType:
         sd->sd_OverscanType = data;
         break;
      case ASLSM_InitialAutoScroll:
      case SCREENREQ_AutoScroll:
         sd->sd_AutoScroll = data;
         break;
      };
   };
   return rc;
}
METHOD_END
///
/// OM_GET
/*
 * Give one of the attributes.
 */
METHOD(ScreenReqClassGet, struct opGet *, opg)
{
   SD          *sd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage, attr = opg->opg_AttrID;

   switch (attr)
   {
   case ASLSM_InitialDisplayID:
   case SCREENREQ_DisplayID:
      STORE sd->sd_DisplayID;
      break;
   case ASLSM_InitialDisplayWidth:
   case SCREENREQ_DisplayWidth:
      STORE sd->sd_DisplayWidth;
      break;
   case ASLSM_InitialDisplayHeight:
   case SCREENREQ_DisplayHeight:
      STORE sd->sd_DisplayHeight;
      break;
   case ASLSM_InitialDisplayDepth:
   case SCREENREQ_DisplayDepth:
      STORE sd->sd_DisplayDepth;
      break;
   case ASLSM_InitialOverscanType:
   case SCREENREQ_OverscanType:
      STORE sd->sd_OverscanType;
      break;
   case ASLSM_InitialAutoScroll:
   case SCREENREQ_AutoScroll:
      STORE sd->sd_AutoScroll;
      break;
  default:
      rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
METHOD_END
///
/// ASLM_DOREQUEST
/*
 * Pop up the ScreenRequester.
 */
METHOD(ScreenReqClassDoRequest, Msg, msg)
{
   SD                         *sd = INST_DATA(cl, obj);
   struct ScreenModeRequester *sm;
   ULONG                       rc = FRQ_OK;
   
   DoSetMethodNG(obj, ASLSM_InitialDisplayWidth,  sd->sd_DisplayWidth,
                      ASLSM_InitialDisplayHeight, sd->sd_DisplayHeight,
                      ASLSM_InitialDisplayDepth,  sd->sd_DisplayDepth,
                      ASLSM_InitialDisplayID,     sd->sd_DisplayID,
                      ASLSM_InitialOverscanType,  sd->sd_OverscanType,
                      ASLSM_InitialAutoScroll,    sd->sd_AutoScroll,
                      TAG_DONE);

   if (sd->sd_InfoWidth)
   {
      DoSetMethodNG(obj, ASLSM_InitialInfoLeftEdge, sd->sd_InfoLeft,
                         ASLSM_InitialInfoTopEdge,  sd->sd_InfoTop,
                         ASLSM_InitialInfoOpened,   sd->sd_InfoOpen,
                         TAG_DONE);
   };

   /*
    * Allocate ScreenRequester structure.
    */
   if (sm = (struct ScreenModeRequester *)AsmDoSuperMethod(cl, obj, ASLM_ALLOCREQUEST))
   {
      /*
       * Put up the requester.
       */
      if (AsmDoSuperMethod(cl, obj, ASLM_REQUEST))
      {
         /*
          * Copy relevant info into our structure.
          */
         DoSetMethodNG(obj, SCREENREQ_DisplayWidth,   sm->sm_DisplayWidth,
                            SCREENREQ_DisplayHeight,  sm->sm_DisplayHeight,
                            SCREENREQ_DisplayDepth,   sm->sm_DisplayDepth,
                            SCREENREQ_DisplayID,      sm->sm_DisplayID,
                            SCREENREQ_OverscanType,   sm->sm_OverscanType,
                            SCREENREQ_AutoScroll,     sm->sm_AutoScroll,
                            TAG_DONE);
      } else
         rc = FRQ_CANCEL;

      /*
       * Remember these even if the requester was cancelled.
       */
      DoSuperSetMethodNG(cl, obj, ASLREQ_Bounds, &sm->sm_LeftEdge, TAG_DONE);

      sd->sd_InfoOpen   = sm->sm_InfoOpened;
      sd->sd_InfoLeft   = sm->sm_InfoLeftEdge;
      sd->sd_InfoTop    = sm->sm_InfoTopEdge;
      sd->sd_InfoWidth  = sm->sm_InfoWidth;
      sd->sd_InfoHeight = sm->sm_InfoHeight;

      /*
       * Free the requester structure.
       */
      AsmDoSuperMethod(cl, obj, ASLM_FREEREQUEST);
   } else
      rc = ASLREQ_ERROR_NO_REQ;

   return rc;
}
METHOD_END
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,                 (FUNCPTR)ScreenReqClassNew,
   OM_SET,                 (FUNCPTR)ScreenReqClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)ScreenReqClassSetUpdate,
   OM_GET,                 (FUNCPTR)ScreenReqClassGet,
   OM_DISPOSE,             (FUNCPTR)ScreenReqClassDispose,
   ASLM_DOREQUEST,         (FUNCPTR)ScreenReqClassDoRequest,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitScreenReqClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ASLREQ_OBJECT,
                         CLASS_ObjectSize,     sizeof(SD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
