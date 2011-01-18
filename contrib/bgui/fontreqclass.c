/*
 * @(#) $Header$
 *
 * BGUI library
 * fontreqclass.c
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
 * Revision 42.0  2000/05/09 22:08:56  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:15  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1999/08/11 02:30:38  mlemos
 * Assured that the initial values passed to the font requester match the
 * defaults of the ASLRequest() call.
 *
 * Revision 41.10  1998/02/25 21:12:02  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:16  mlemos
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
   struct TextAttr    fd_TextAttr;                    /* Selected font     */
   UBYTE              fd_Name[MAX_FILE + 2];          /* Fontname buffer   */
   UBYTE              fd_FrontPen;                    /* Selected FrontPen */
   UBYTE              fd_BackPen;                     /* Selected BackPen  */
   UBYTE              fd_DrawMode;                    /* Selected DrawMode */
}  FD;
///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(FontReqClassNew, struct opSet *, ops)
{
   ULONG           rc;
   struct TagItem *tags;

   tags = DefTagList(BGUI_FONTREQ_OBJECT, ops->ops_AttrList);

   /*
    * First we let the superclass create the object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      FD  *fd = INST_DATA(cl, rc);

      fd->fd_Name[0]='\0';
      fd->fd_TextAttr.ta_YSize=8;
      fd->fd_TextAttr.ta_Style=FS_NORMAL;
      fd->fd_TextAttr.ta_Flags=FPF_ROMFONT;
      fd->fd_FrontPen=1;
      fd->fd_BackPen=0;
      fd->fd_DrawMode=JAM1;

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
METHOD(FontReqClassDispose, Msg, msg)
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
METHOD(FontReqClassSetUpdate, struct opSet *, ops)
{
   FD             *fd = INST_DATA(cl, obj);
   struct TagItem *tag, *tstate = ops->ops_AttrList;
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
      case ASLFO_InitialName:
      case FONTREQ_Name:
         fd->fd_Name[0] = 0;
         if (data) strncpy(fd->fd_Name, (char *)data, MAX_FILE);
         break;
      case ASLFO_InitialSize:
      case FONTREQ_Size:
         fd->fd_TextAttr.ta_YSize = data;
         break;
      case ASLFO_InitialStyle:
      case FONTREQ_Style:
         fd->fd_TextAttr.ta_Style = data;
         break;
      case ASLFO_InitialFlags:
      case FONTREQ_Flags:
         fd->fd_TextAttr.ta_Flags = data;
         break;
      case ASLFO_InitialFrontPen:
      case FONTREQ_FrontPen:
         fd->fd_FrontPen = data;
         break;
      case ASLFO_InitialBackPen:
      case FONTREQ_BackPen:
         fd->fd_BackPen = data;
         break;
      case ASLFO_InitialDrawMode:
      case FONTREQ_DrawMode:
         fd->fd_DrawMode = data;
         break;
      case FONTREQ_TextAttr:
         fd->fd_Name[0] = 0;
         if (data)
         {
            fd->fd_TextAttr = *((struct TextAttr *)data);
            if (fd->fd_TextAttr.ta_Name)
               strncpy(fd->fd_Name, fd->fd_TextAttr.ta_Name, MAX_FILE);
            fd->fd_TextAttr.ta_Name = fd->fd_Name;
         };
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
METHOD(FontReqClassGet, struct opGet *, opg)
{
   FD          *fd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage, attr = opg->opg_AttrID;

   switch (attr)
   {
   case ASLFO_InitialName:
   case FONTREQ_Name:
      STORE fd->fd_Name;
      break;
   case ASLFO_InitialSize:
   case FONTREQ_Size:
      STORE fd->fd_TextAttr.ta_YSize;
      break;
   case ASLFO_InitialStyle:
   case FONTREQ_Style:
      STORE fd->fd_TextAttr.ta_Style;
      break;
   case ASLFO_InitialFlags:
   case FONTREQ_Flags:
      STORE fd->fd_TextAttr.ta_Flags;
      break;
   case ASLFO_InitialFrontPen:
   case FONTREQ_FrontPen:
      STORE fd->fd_FrontPen;
      break;
   case ASLFO_InitialBackPen:
   case FONTREQ_BackPen:
      STORE fd->fd_BackPen;
      break;
   case ASLFO_InitialDrawMode:
   case FONTREQ_DrawMode:
      STORE fd->fd_DrawMode;
      break;
   case FONTREQ_TextAttr:
      STORE &fd->fd_TextAttr;
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
 * Pop up the FontRequester.
 */
METHOD(FontReqClassDoRequest, Msg, msg)
{
   FD                   *fd = INST_DATA(cl, obj);
   struct FontRequester *fo;
   ULONG                 rc = ASLREQ_OK;
   
   DoSuperSetMethodNG(cl, obj, ASLFO_InitialName,      fd->fd_Name,
                               ASLFO_InitialSize,      fd->fd_TextAttr.ta_YSize,
                               ASLFO_InitialStyle,     fd->fd_TextAttr.ta_Style,
                               ASLFO_InitialFlags,     fd->fd_TextAttr.ta_Flags,
                               ASLFO_InitialFrontPen,  fd->fd_FrontPen,
                               ASLFO_InitialBackPen,   fd->fd_BackPen,
                               ASLFO_InitialDrawMode,  fd->fd_DrawMode, TAG_DONE);

   /*
    * Allocate FontRequester structure.
    */
   if (fo = (struct FontRequester *)AsmDoSuperMethod(cl, obj, ASLM_ALLOCREQUEST))
   {
      /*
       * Put up the requester.
       */
      if (AsmDoSuperMethod(cl, obj, ASLM_REQUEST))
      {
         /*
          * Copy font information.
          */
         DoSetMethodNG(obj, FONTREQ_TextAttr,  &fo->fo_Attr,
                            FONTREQ_FrontPen,  fo->fo_FrontPen,
                            FONTREQ_BackPen,   fo->fo_BackPen,
                            FONTREQ_DrawMode,  fo->fo_DrawMode, TAG_DONE);
      } else
         rc = ASLREQ_CANCEL;
      /*
       * Remember these even if the requester was cancelled.
       */
      DoSuperSetMethodNG(cl, obj, ASLREQ_Bounds, &fo->fo_LeftEdge, TAG_DONE);

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
   OM_NEW,                 (FUNCPTR)FontReqClassNew,
   OM_SET,                 (FUNCPTR)FontReqClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)FontReqClassSetUpdate,
   OM_GET,                 (FUNCPTR)FontReqClassGet,
   OM_DISPOSE,             (FUNCPTR)FontReqClassDispose,
   ASLM_DOREQUEST,         (FUNCPTR)FontReqClassDoRequest,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitFontReqClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ASLREQ_OBJECT,
                         CLASS_ObjectSize,     sizeof(FD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
