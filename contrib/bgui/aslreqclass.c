/*
 * @(#) $Header$
 *
 * BGUI library
 * aslreqclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 */

#include "include/classdefs.h"

#ifdef __AROS__
extern struct Library * AslBase;
#endif

/// Class definitions.
/*
 * Object instance data.
 */
typedef struct ad_ {
   APTR            ad_Requester; /* Requester         */
   ULONG           ad_Type;      /* Requester type    */
   WORD            ad_Left;      /* Requester left    */
   WORD            ad_Top;       /* Requester top     */
   UWORD           ad_Width;     /* Requester width   */
   UWORD           ad_Height;    /* Requester height  */
   struct TagItem *ad_ASLTags;   /* Copy of ASL tags  */
}  AD;

#define AD_ENTRY(tag, offset, flags) PACK_ENTRY(ASLREQ_TAGSTART, tag, ad_, offset, flags)
#define AD_FLAG(tag, flag) PACK_LONGBIT(ASLREQ_TAGSTART, tag, ad_, ad_Flags, PKCTRL_BIT, flag)

static ULONG AslPackTable[] =
{
   PACK_STARTTABLE(ASLREQ_TAGSTART),

   AD_ENTRY(ASLREQ_Requester,    ad_Requester,  PKCTRL_ULONG),
   AD_ENTRY(ASLREQ_Type,         ad_Type,       PKCTRL_ULONG),
   AD_ENTRY(ASLREQ_Left,         ad_Left,       PKCTRL_WORD),
   AD_ENTRY(ASLREQ_Top,          ad_Top,        PKCTRL_WORD),
   AD_ENTRY(ASLREQ_Width,        ad_Width,      PKCTRL_UWORD),
   AD_ENTRY(ASLREQ_Height,       ad_Height,     PKCTRL_UWORD),

   PACK_ENDTABLE
};
///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(AslReqClassNew, struct opSet *, ops)
{
   AD          *ad;
   ULONG        rc;

   /*
    * First we let the superclass
    * create the object.
    */
   if (rc = AsmDoSuperMethodA(cl, obj, (Msg)ops))
   {
      /*
       * Get us the instance data.
       */
      ad = INST_DATA(cl, rc);

      /*
       * Setup the defaults.
       */
      ad->ad_Left          = 30;
      ad->ad_Top           = 20;
      ad->ad_Width         = 320;
      ad->ad_Height        = 180;
      ad->ad_ASLTags       = CloneTagItems(NULL);

      /*
       * Setup user attributes.
       */
      if (AsmCoerceMethod(cl, (Object *)rc, OM_SET, ops, NULL) != ASLREQ_OK)
      {
         /*
          * Failure.
          */
         AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
         rc = 0;
      }
   }
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * They want us gone.
 */
METHOD(AslReqClassDispose, Msg, msg)
{
   AD       *ad = INST_DATA(cl, obj);

   /*
    * Free requester, if it still exists somehow.
    */
   FreeAslRequest(ad->ad_Requester);

   /*
    * Free tag copy.
    */
   FreeTagItems(ad->ad_ASLTags);

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
METHOD(AslReqClassSetUpdate, struct opSet *, ops)
{
   AD             *ad = INST_DATA(cl, obj);
   struct TagItem *tstate, *tags, *tag;
   ULONG           attr, data;
   BOOL            asl = FALSE;

   /*
    * Filter out values in old taglist that will be replaced,
    * and values in new taglist that are not ASL tags.
    * Process the non-ASL tags before disposal.
    */
   tstate = tags = CloneTagItems(ops->ops_AttrList);

   while (tag = NextTagItem(&tstate))
   {
      attr = tag->ti_Tag;
      data = tag->ti_Data;

      if ((attr >= ASL_TB) && (attr < ASL_TB + 0x10000))
      {
         asl = TRUE;
      }
      else
      {
         switch (attr)
         {
         case ASLREQ_Bounds:
            *((struct IBox *)&ad->ad_Left) = *((struct IBox *)data);
            break;
         default:
            /*
             * Try to process it.
             */
            BGUI_PackStructureTag((UBYTE *)ad, AslPackTable, attr, data);
            break;
         };
         /*
          * Kill it.
          */
         tag->ti_Tag = TAG_IGNORE;
      };
   };

   /*
    * Merge the old with the new.
    */
   if (asl)
   {
      tag = BGUI_MergeTagItems(ad->ad_ASLTags, tags);
      FreeTagItems(ad->ad_ASLTags);
      ad->ad_ASLTags = BGUI_CleanTagItems(tag, 1);
   };

   FreeTagItems(tags);

   return ASLREQ_OK;
}
METHOD_END
///
/// OM_GET
/*
 * Give one of the attributes.
 */
METHOD(AslReqClassGet, struct opGet *, opg)
{
   AD                *ad = INST_DATA(cl, obj);
   struct TagItem    *tag;
   ULONG              rc, attr = opg->opg_AttrID, *store = opg->opg_Storage;
   
   if (tag = FindTagItem(attr, ad->ad_ASLTags))
   {
      *store = tag->ti_Data;
      rc = 1;
   }
   else
   {
      rc = BGUI_UnpackStructureTag((UBYTE *)ad, AslPackTable, attr, store);
      if (!rc) rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
   };
   return rc;
}
METHOD_END
///
/// ASLM_ALLOCREQUEST
/*
 * Allocate requester.
 */
METHOD(AslReqClassAllocRequest, Msg, msg)
{
   AD          *ad = INST_DATA(cl, obj);
   
   ad->ad_Requester = AllocAslRequestTags(ad->ad_Type,
      ASL_LeftEdge,     ad->ad_Left,      ASL_TopEdge,   ad->ad_Top,
      ASL_Width,        ad->ad_Width,     ASL_Height,    ad->ad_Height,
      TAG_DONE);
      
   return (ULONG)ad->ad_Requester;
}
METHOD_END
///
/// ASLM_REQUEST
/*
 * Show requester.
 */
METHOD(AslReqClassRequest, Msg, msg)
{
   AD          *ad = INST_DATA(cl, obj);
   ULONG        rc = 0;
   
   if (ad->ad_Requester)
   {
      rc = AslRequest(ad->ad_Requester, ad->ad_ASLTags);
   };
   return rc;
}
METHOD_END
///
/// ASLM_FREEREQUEST
/*
 * Free requester.
 */
METHOD(AslReqClassFreeRequest, Msg, msg)
{
   AD          *ad = INST_DATA(cl, obj);
   
   FreeAslRequest(ad->ad_Requester);
   ad->ad_Requester = NULL;
   
   return 1;
}
METHOD_END
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,              (FUNCPTR)AslReqClassNew,
   OM_SET,              (FUNCPTR)AslReqClassSetUpdate,
   OM_UPDATE,           (FUNCPTR)AslReqClassSetUpdate,
   OM_GET,              (FUNCPTR)AslReqClassGet,
   OM_DISPOSE,          (FUNCPTR)AslReqClassDispose,
   ASLM_ALLOCREQUEST,   (FUNCPTR)AslReqClassAllocRequest,
   ASLM_REQUEST,        (FUNCPTR)AslReqClassRequest,
   ASLM_FREEREQUEST,    (FUNCPTR)AslReqClassFreeRequest,
   DF_END,              NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitAslReqClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ROOT_OBJECT,
                         CLASS_ObjectSize,     sizeof(AD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
