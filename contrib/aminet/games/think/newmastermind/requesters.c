/*
 * requesters.c
 * ============
 * Some usefull functions for handling requesters.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/alib.h>

extern struct IntuitionBase  *IntuitionBase;
extern struct GfxBase * GfxBase;
extern struct Library * GadToolsBase;

BOOL
window_sleep (
   struct Window     *win,
   struct Requester  *req)
{
   InitRequester (req);
   if (Request (req, win))
   {
      if (((struct Library *)IntuitionBase)->lib_Version >= 39L)
         SetWindowPointer (win, WA_BusyPointer, TRUE, TAG_DONE);
      
      return TRUE;
   }
   
   return FALSE;
}

void
window_wakeup (
   struct Window     *win,
   struct Requester  *req)
{
   if (((struct Library *)IntuitionBase)->lib_Version >= 39L)
      SetWindowPointer (win, WA_Pointer, NULL, TAG_DONE);
   EndRequest (req, win);
}

void
msg_requester (
   struct Window  *win,
   char           *title,
   char           *gad_title,
   char           *message)
{
   struct EasyStruct   msg_req;
   struct Requester    req;
   BOOL   win_sleep = FALSE;
   
   msg_req.es_StructSize = sizeof (msg_req);
   msg_req.es_Flags = 0;
   msg_req.es_Title = title;
   msg_req.es_TextFormat = message;
   msg_req.es_GadgetFormat = gad_title;
   
   if (win != NULL)
      win_sleep = window_sleep (win, &req);
   EasyRequest (win, &msg_req, NULL, NULL);
   if (win_sleep)
      window_wakeup (win, &req);
}

void
string_requester (
   struct Window  *win,
   APTR            vis_info,
   char           *win_title,
   char           *gad_title,
   char           *buffer,
   UBYTE           buf_size)
{
   struct Window  *req_win;
   struct NewGadget   new_gad;
   struct Gadget  *str_gad, *gad_list;
   struct IntuiMessage  *msg;
   BOOL   done = FALSE;
   struct Requester    req;
   BOOL   win_sleep;

   win_sleep = window_sleep (win, &req);
   
   str_gad = CreateContext (&gad_list);
   new_gad.ng_TextAttr = win->WScreen->Font;
   new_gad.ng_VisualInfo = vis_info;
   new_gad.ng_LeftEdge = win->WScreen->WBorLeft + 16 +
                         TextLength (&win->WScreen->RastPort,
                                     gad_title, strlen (gad_title));
   new_gad.ng_TopEdge = win->WScreen->WBorTop +
                        win->WScreen->Font->ta_YSize + 5;
   new_gad.ng_Width = 200;
   new_gad.ng_Height = win->WScreen->Font->ta_YSize + 6;
   new_gad.ng_GadgetText = gad_title;
   new_gad.ng_GadgetID = 1;
   new_gad.ng_Flags = 0;
   str_gad = CreateGadget (STRING_KIND, str_gad, &new_gad,
                           GTST_String, buffer,
                           GTST_MaxChars, buf_size,
                           TAG_DONE);
   if (str_gad)
   {
      req_win = OpenWindowTags (NULL,
                                WA_Left, win->LeftEdge +
                                         (win->Width -
                                          win->BorderLeft -
                                          win->BorderRight -
                                          str_gad->LeftEdge -
                                          str_gad->Width - 14) / 2,
                                WA_Top, win->TopEdge +
                                        (win->Height -
                                         win->BorderTop -
                                         win->BorderBottom -
                                         str_gad->Height - 14) / 2,
                                WA_InnerWidth, str_gad->LeftEdge +
                                               str_gad->Width + 14,
                                WA_InnerHeight, str_gad->Height + 14,
                                WA_Title, win_title,
                                WA_ScreenTitle, win->ScreenTitle,
                                WA_PubScreen, win->WScreen,
                                WA_Gadgets, gad_list,
                                WA_IDCMP, STRINGIDCMP |
                                          IDCMP_REFRESHWINDOW,
                                WA_DragBar, TRUE,
                                WA_DepthGadget, TRUE,
                                WA_Activate, TRUE,
                                TAG_DONE);
      if (req_win)
      {
         GT_RefreshWindow (req_win, NULL);
         ActivateGadget (str_gad, req_win, NULL);
         
         while (!done)
         {
            WaitPort (req_win->UserPort);
            while (msg = GT_GetIMsg (req_win->UserPort))
            {
               switch (msg->Class)
               {
               case IDCMP_GADGETUP:
                  done = TRUE;
                  break;
               case IDCMP_REFRESHWINDOW:
                  GT_BeginRefresh (req_win);
                  GT_EndRefresh (req_win, TRUE);
                  break;
               }
               GT_ReplyIMsg (msg);
            }
         }
         strncpy (buffer,
                  ((struct StringInfo *)str_gad->SpecialInfo)->Buffer,
                  buf_size);
         CloseWindow (req_win);
      }
      FreeGadgets (gad_list);
   }
   if (win_sleep)
      window_wakeup (win, &req);
}
