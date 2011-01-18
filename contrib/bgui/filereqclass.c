/*
 * @(#) $Header$
 *
 * BGUI library
 * filereqclass.c
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
 * Revision 42.0  2000/05/09 22:08:53  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:14  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.1  1999/07/03 15:17:35  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10  1998/02/25 21:12:00  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:12  mlemos
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
   struct Hook       *fd_MultiHook;                   /* Multi-select hook */
   UBYTE              fd_Drawer [MAX_DRAWER  + 2];    /* Drawer buffer     */
   UBYTE              fd_File   [MAX_FILE    + 2];    /* Filename buffer   */
   UBYTE              fd_Path   [MAX_PATH    + 2];    /* Full path buffer  */
   UBYTE              fd_Pattern[MAX_PATTERN + 2];    /* Pattern buffer    */
}  FD;
///

/// OM_NEW
/*
 * Create a shiny new object.
 */
METHOD(FileReqClassNew, struct opSet *, ops)
{
   ULONG           rc;
   struct TagItem *tags;

   tags = DefTagList(BGUI_FILEREQ_OBJECT, ops->ops_AttrList);

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
METHOD(FileReqClassDispose, Msg, msg)
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
METHOD(FileReqClassSetUpdate, struct opSet *, ops)
{
   FD             *fd = INST_DATA(cl, obj);
   struct TagItem *tag, *tstate = ops->ops_AttrList;
   ULONG           data, rc;
   BOOL            path_update = FALSE;

   /*
    * First we let the superclass have a go at it.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)ops);

   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case FILEREQ_MultiHook:
         fd->fd_MultiHook = (struct Hook *)data;
         break;
      case ASLFR_InitialFile:
      case FILEREQ_File:
         strncpy(fd->fd_File, (UBYTE *)data, MAX_FILE);
         path_update = TRUE;
         break;
      case ASLFR_InitialDrawer:
      case FILEREQ_Drawer:
         strncpy(fd->fd_Drawer, (UBYTE *)data, MAX_DRAWER);
         path_update = TRUE;
         break;
      case ASLFR_InitialPattern:
      case FILEREQ_Pattern:
         strncpy(fd->fd_Pattern, (UBYTE *)data, MAX_PATTERN);
         break;
      };
      if (path_update)
      {
         strncpy(fd->fd_Path, fd->fd_Drawer, MAX_PATH);
         AddPart(fd->fd_Path, fd->fd_File,   MAX_PATH);
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
METHOD(FileReqClassGet, struct opGet *, opg)
{
   FD          *fd = INST_DATA(cl, obj);
   ULONG        rc = 1, *store = opg->opg_Storage, attr = opg->opg_AttrID;

   switch (attr)
   {
   case ASLFR_InitialDrawer:
   case FILEREQ_Drawer:
      STORE fd->fd_Drawer;
      break;
   case ASLFR_InitialFile:
   case FILEREQ_File:
      STORE fd->fd_File;
      break;
   case ASLFR_InitialPattern:
   case FILEREQ_Pattern:
      STORE fd->fd_Pattern;
      break;
   case FILEREQ_Path:
      STORE fd->fd_Path;
      break;
   case FILEREQ_MultiHook:
      STORE fd->fd_MultiHook;
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
 * Pop up the filerequester.
 */
METHOD(FileReqClassDoRequest, Msg, msg)
{
   FD                   *fd = INST_DATA(cl, obj);
   struct FileRequester *fr;
   ULONG                 rc = ASLREQ_OK;
   
   DoSuperSetMethodNG(cl, obj, ASLFR_InitialFile,     fd->fd_File,
                               ASLFR_InitialDrawer,   fd->fd_Drawer,
                               ASLFR_InitialPattern,  fd->fd_Pattern, TAG_DONE);

   /*
    * Allocate filerequester structure.
    */
   if (fr = (struct FileRequester *)AsmDoSuperMethod(cl, obj, ASLM_ALLOCREQUEST))
   {
      /*
       * Put up the requester.
       */
      if (AsmDoSuperMethod(cl, obj, ASLM_REQUEST))
      {
         /*
          * Copy drawer, file and pattern strings.
          */
         DoSetMethodNG(obj, FILEREQ_File,     fr->fr_File,
                            FILEREQ_Drawer,   fr->fr_Drawer,
                            FILEREQ_Pattern,  fr->fr_Pattern, TAG_DONE);

         /*
          * Do we have a multi-select hook?
          */
         if (fd->fd_MultiHook)
         {
            /*
             * Any multi-selections made?
             */
            if (fr->fr_NumArgs)
               BGUI_CallHookPkt(fd->fd_MultiHook, (VOID *)obj, (VOID *)fr);
         };
         
      } else
         rc = ASLREQ_CANCEL;

      /*
       * Remember these even if the requester was cancelled.
       */
      DoSuperSetMethodNG(cl, obj, ASLREQ_Bounds, &fr->fr_LeftEdge, TAG_DONE);

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
   OM_NEW,                 (FUNCPTR)FileReqClassNew,
   OM_SET,                 (FUNCPTR)FileReqClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)FileReqClassSetUpdate,
   OM_GET,                 (FUNCPTR)FileReqClassGet,
   OM_DISPOSE,             (FUNCPTR)FileReqClassDispose,
   ASLM_DOREQUEST,         (FUNCPTR)FileReqClassDoRequest,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitFileReqClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_ASLREQ_OBJECT,
                         CLASS_ObjectSize,     sizeof(FD),
                         CLASS_DFTable,        ClassFunc,
                         TAG_DONE);
}
///
