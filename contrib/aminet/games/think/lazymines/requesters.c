/*
 * requesters.c
 * ============
 * Some usefull functions for handling requesters.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/types.h>
#include <proto/exec.h>
#include <libraries/gadtools.h>
#include <proto/gadtools.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <proto/graphics.h>
#include <intuition/imageclass.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <proto/intuition.h>
#include <proto/alib.h>

#include "display_globals.h"
#include "localize.h"
#include "game.h"
#include "requesters.h"


extern APTR   vis_info;

extern struct GfxBase * GfxBase;
extern struct GadToolsBase * GadToolsBase;

static UWORD   dither_data[] = { 0xAAAA, 0x5555 };


BOOL
window_sleep (
   struct Window     *win,
   struct Requester  *req)
{
   InitRequester (req);
   if (Request (req, win))
   {
      if (IntuitionBase->LibNode.lib_Version >= 39L)
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
   if (IntuitionBase->LibNode.lib_Version >= 39L)
      SetWindowPointer (win, WA_Pointer, NULL, TAG_DONE);
   EndRequest (req, win);
}

void
error_requester (
   struct Window  *win,
   ULONG           msg,
   char           *more)
{
   struct EasyStruct   msg_req;
   struct Requester    req;
   BOOL   win_sleep = FALSE;
   
   
   if (IntuitionBase != NULL)
   {
      msg_req.es_StructSize = sizeof (msg_req);
      msg_req.es_Flags = 0;
      msg_req.es_Title = localized_string (MSG_ERROR_REQTITLE);
      msg_req.es_TextFormat = localized_string (msg);
      msg_req.es_GadgetFormat = localized_string (MSG_OK_GAD);
      
      if (win != NULL)
         win_sleep = window_sleep (win, &req);
      EasyRequest (win, &msg_req, NULL, more);
      if (win_sleep)
         window_wakeup (win, &req);
   }
   else
      fprintf (stderr, localized_string (msg), more);
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

   str_gad = (struct Gadget *)CreateContext (&gad_list);
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
         win_sleep = window_sleep (win, &req);
   
         GT_RefreshWindow (req_win, NULL);
         ActivateGadget (str_gad, req_win, NULL);
         
         while (!done)
         {
            WaitPort (req_win->UserPort);
            while ((msg = GT_GetIMsg (req_win->UserPort)))
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
         if (win_sleep)
            window_wakeup (win, &req);
         CloseWindow (req_win);
      }
      FreeGadgets (gad_list);
   }
}


void
about_requester (
   struct Window  *win,
   struct Image   *img,
   STRPTR          name,
   STRPTR          version,
   STRPTR          msg_str)
{
   struct RastPort     layout_rp;
   struct TextExtent   te;
   ULONG               box_w = 0, box_h = 0, win_w, win_h;
   LONG                temp;
   STRPTR              name_str, ver_str, pos, next, real_pos;
   
   struct NewGadget    ng;
   struct Gadget      *gad_list, *ok_gad;
   struct Window      *req_win;
   struct Requester    req;
   BOOL                win_sleep, done = FALSE;
   
   
   if ((name_str = malloc (2 * strlen (name) * sizeof (*name_str))) &&
       (ver_str = malloc ((strlen (version) + 2) * sizeof (*name_str))))
   {
      for (temp = 0; temp < 2 * strlen (name) - 1; ++temp)
         name_str[temp] = (temp & 1) ? ' ' : name[temp / 2];
      name_str[temp] = '\0';
      ver_str[0] = 'v';
      strcpy (ver_str + 1, version);
      
      InitRastPort (&layout_rp);
      SetFont (&layout_rp, win->WScreen->RastPort.Font);
      /* Image extent */
      if (img != NULL)
      {
         GetAttr (IA_Width, (Object *)img, &box_w);
         GetAttr (IA_Height, (Object *)img, &box_h);
      }
      /* Name extent */
      SetSoftStyle (&layout_rp,
                    FSF_BOLD | FSF_ITALIC, AskSoftStyle (&layout_rp));
      TextExtent (&layout_rp, name_str, strlen (name_str), &te);
      box_w += te.te_Extent.MaxX + 1 + INTERWIDTH / 2;
      if (te.te_Extent.MaxY - te.te_Extent.MinY + 1 > box_h)
         box_h = te.te_Extent.MaxY - te.te_Extent.MinY + 1;
      /* Version extent */
      SetSoftStyle (&layout_rp, FS_NORMAL, AskSoftStyle (&layout_rp));
      TextExtent (&layout_rp, ver_str, strlen (ver_str), &te);
      box_w += te.te_Extent.MaxX + 1 + INTERWIDTH / 2;
      box_h += INTERHEIGHT + LINEHEIGHT;
      /* Msg extent */
      temp = 0;
      real_pos = pos = msg_str;
      while (pos != NULL)
      {
         next = strpbrk (pos, "()\n");
         if (next == NULL || *next == '\n')
         {
            TextExtent (&layout_rp, real_pos,
                        ((next == NULL) ? strlen (real_pos) :
                                          next - real_pos),
                        &te);
            if (te.te_Width > 0)
               temp += te.te_Extent.MaxX + 1;
            if (temp > box_w)
               box_w = temp;
            if (te.te_Height > 0)
               box_h += te.te_Extent.MaxY - te.te_Extent.MinY + 1;
            else
               box_h += layout_rp.TxHeight;
            temp = 0;
            real_pos = next + 1;
         }
         else if (*next == '(')
         {
            TextExtent (&layout_rp, real_pos, next - real_pos, &te);
            temp += te.te_Extent.MaxX + 1;
            SetSoftStyle (&layout_rp, FSF_ITALIC, AskSoftStyle (&layout_rp));
            real_pos = next;
         }
         else if (*next == ')')
         {
            TextExtent (&layout_rp, real_pos, next - real_pos + 1, &te);
            temp += te.te_Extent.MaxX + 1;
            SetSoftStyle (&layout_rp, FS_NORMAL, AskSoftStyle (&layout_rp));
            real_pos = next + 1;
         }
         if (next != NULL)
            ++next;
         pos = next;
      }
      /* Final extent */
      box_w += 2 * (INTERWIDTH + LINEWIDTH);
      box_h += 2 * (INTERHEIGHT + LINEHEIGHT);
      win_w = box_w;
      
      ng.ng_GadgetText = localized_string (MSG_CONTINUE_GAD);
      ng.ng_Width = TextLength (&win->WScreen->RastPort,
                                ng.ng_GadgetText, strlen (ng.ng_GadgetText)) +
                    INTERWIDTH + 2 * LINEWIDTH;
      if (ng.ng_Width > box_w)
         win_w = ng.ng_Width;
      win_w += win->BorderLeft + win->BorderRight + 2 * INTERWIDTH;
      ng.ng_Height = win->WScreen->Font->ta_YSize +
                     INTERHEIGHT + 2 * LINEHEIGHT;
      win_h = win->BorderTop + win->BorderBottom +
              box_h + ng.ng_Height + 3 * INTERHEIGHT;
      ng.ng_LeftEdge = (win_w - ng.ng_Width) / 2;
      ng.ng_TopEdge = win->BorderTop + box_h + 2 * INTERHEIGHT;
      ng.ng_TextAttr = win->WScreen->Font;
      ng.ng_GadgetID = 1;
      ng.ng_Flags = 0;
      ng.ng_VisualInfo = vis_info;
      ng.ng_UserData = 0;
      ok_gad = CreateContext (&gad_list);
      ok_gad = CreateGadget (BUTTON_KIND, ok_gad, &ng, TAG_DONE);
      
      if (ok_gad)
      {
         req_win = OpenWindowTags (NULL,
                                   WA_Left, win->LeftEdge +
                                            (win->Width - win_w) / 2,
                                   WA_Top, win->TopEdge +
                                           (win->Height - win_h) / 2,
                                   WA_Width, win_w,
                                   WA_Height, win_h,
                                   WA_Title,
                                      localized_string (MSG_ABOUT_REQTITLE),
                                   WA_ScreenTitle, win->ScreenTitle,
                                   WA_PubScreen, win->WScreen,
                                   WA_IDCMP, BUTTONIDCMP |
                                             IDCMP_REFRESHWINDOW,
                                   WA_DragBar, TRUE,
                                   WA_DepthGadget, TRUE,
                                   WA_Activate, TRUE,
                                   TAG_DONE);
         if (req_win)
         {
            ULONG   winsig, timersig, sigmask;
            struct IntuiMessage  *msg;
            
            
            win_sleep = window_sleep (win, &req);
            SetAPen (req_win->RPort, gui_pens[SHINEPEN]);
            SetAfPt (req_win->RPort, dither_data, 1);
            RectFill (req_win->RPort, req_win->BorderLeft, req_win->BorderTop,
                      req_win->Width - req_win->BorderRight - 1,
                      req_win->Height - req_win->BorderBottom - 1);
            AddGList (req_win, gad_list, -1, -1, NULL);
            RefreshGList (gad_list, req_win, NULL, -1);
            GT_RefreshWindow (req_win, NULL);
            SetAPen (req_win->RPort, gui_pens[BACKGROUNDPEN]);
            SetAfPt (req_win->RPort, NULL, 0);
            RectFill (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                      req_win->BorderTop + INTERHEIGHT,
                      req_win->BorderLeft + INTERWIDTH + box_w - 1,
                      req_win->BorderTop + INTERHEIGHT + box_h - 1);
            DrawBevelBox (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                          req_win->BorderTop + INTERHEIGHT, box_w, box_h,
                          GT_VisualInfo, vis_info, GTBB_Recessed, TRUE,
                          TAG_DONE);
            SetAPen (req_win->RPort, gui_pens[TEXTPEN]);
            SetFont (req_win->RPort, win->WScreen->RastPort.Font);
            SetSoftStyle (req_win->RPort, FSF_BOLD | FSF_ITALIC,
                          AskSoftStyle (req_win->RPort));
            TextExtent (req_win->RPort, name_str, strlen (name_str), &te);
            temp = te.te_Extent.MaxY - te.te_Extent.MinY + 1;
            if (img != NULL)
            {
               GetAttr (IA_Height, (Object *)img, &win_h);
               if (win_h > temp)
                  temp = win_h;
               DrawImageState (req_win->RPort, img,
                               req_win->BorderLeft +
                               2 * INTERWIDTH + LINEWIDTH,
                               req_win->BorderTop + 2 * INTERHEIGHT +
                               LINEHEIGHT + (temp - win_h) / 2,
                               IDS_NORMAL, NULL);
               
               GetAttr (IA_Width, (Object *)img, &win_w);
            }
            Move (req_win->RPort,
                  req_win->BorderLeft + 2 * INTERWIDTH + LINEWIDTH +
                  ((img != NULL) ? win_w + INTERWIDTH / 2 : 0),
                  req_win->BorderTop + 2 * INTERHEIGHT + LINEHEIGHT +
                  (temp - te.te_Extent.MaxY + te.te_Extent.MinY - 1) / 2 +
                  req_win->RPort->TxBaseline);
            Text (req_win->RPort, name_str, strlen (name_str));
            SetSoftStyle (req_win->RPort,
                          FS_NORMAL, AskSoftStyle (req_win->RPort));
            TextExtent (req_win->RPort, ver_str, strlen (ver_str), &te);
            Move (req_win->RPort,
                  req_win->Width - req_win->BorderRight - 2 * INTERWIDTH -
                  LINEWIDTH - te.te_Extent.MaxX - 1,
                  req_win->BorderTop + 2 * INTERHEIGHT + LINEHEIGHT +
                  temp + INTERHEIGHT / 2 - 1);
            Text (req_win->RPort, ver_str, strlen (ver_str));
            win_w = req_win->BorderLeft + 2 * INTERWIDTH + LINEWIDTH;
            win_h = req_win->BorderTop + 3 * INTERHEIGHT + 2 * LINEHEIGHT +
                    temp;
            for (temp = 0; temp < LINEHEIGHT; ++temp)
            {
               Move (req_win->RPort, win_w,
                     win_h - INTERHEIGHT / 2 - LINEHEIGHT + temp);
               Draw (req_win->RPort,
                     req_win->Width - req_win->BorderRight - 2 * INTERWIDTH -
                     LINEWIDTH - 1,
                     win_h - INTERHEIGHT / 2 - LINEHEIGHT + temp);
            }
      
            Move (req_win->RPort, win_w, win_h + req_win->RPort->TxBaseline);
            real_pos = pos = msg_str;
            while (pos != NULL)
            {
               next = strpbrk (pos, "()\n");
               if (next == NULL || *next == '\n')
               {
                  TextExtent (req_win->RPort, real_pos,
                              ((next == NULL) ? strlen (real_pos) :
                                                next - real_pos),
                              &te);
                  Text (req_win->RPort, real_pos,
                        ((next == NULL) ?
                         strlen (real_pos) : next - real_pos));
                  if (te.te_Height > 0)
                     win_h += te.te_Extent.MaxY - te.te_Extent.MinY + 1;
                  else
                     win_h += req_win->RPort->TxHeight;
                  Move (req_win->RPort,
                        win_w, win_h + req_win->RPort->TxBaseline);
                  real_pos = next + 1;
               }
               else if (*next == '(')
               {
                  Text (req_win->RPort, real_pos, next - real_pos);
                  SetSoftStyle (req_win->RPort,
                                FSF_ITALIC, AskSoftStyle (req_win->RPort));
                  real_pos = next;
               }
               else if (*next == ')')
               {
                  Text (req_win->RPort, real_pos, next - real_pos + 1);
                  SetSoftStyle (req_win->RPort,
                                 FS_NORMAL, AskSoftStyle (req_win->RPort));
                  real_pos = next + 1;
               }
               if (next != NULL)
                  ++next;
               pos = next;
            }
            
            winsig = 1L << req_win->UserPort->mp_SigBit;
            timersig = timer_signal (timer_obj);
            while (!done)
            {
               sigmask = Wait (winsig | timersig);
               if (sigmask & winsig)
               {
                  while (NULL !=(msg = GT_GetIMsg (req_win->UserPort)))
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
               if (sigmask & timersig)
               {
                  timer_reply (timer_obj);
                  if (time_on)
                  {
                     timer_start (timer_obj, 0L, 1000000L);
                     counter_update (time_counter,
                                     counter_value (time_counter) + 1);
                  }
               }
            }
            if (win_sleep)
               window_wakeup (win, &req);
            CloseWindow (req_win);
         }
         FreeGadgets (gad_list);
      }
   }
   if (name_str)
      free (name_str);
   if (ver_str)
      free (ver_str);
}


void
request_optional_size (
   struct Window  *win)
{
   struct RastPort   layout_rp;
   struct TextAttr   ta;
   ULONG             box_w, box_h, win_w, win_h, temp;
   STRPTR            label;
   UWORD             max_mines;
      
   struct NewGadget    ng1, ng2, ng3, ng4;
   struct Gadget      *gad_list, *slider1, *slider2, *slider3, *ok_gad;
   struct Window      *req_win;
   struct Requester    req;
   BOOL                win_sleep, done = FALSE;
   
   
   InitRastPort (&layout_rp);
   ta.ta_Name = GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
   ta.ta_YSize = GfxBase->DefaultFont->tf_YSize;
   ta.ta_Style = GfxBase->DefaultFont->tf_Style;
   ta.ta_Flags = GfxBase->DefaultFont->tf_Flags;
   
   label = localized_string (MSG_NUMROWS_GAD);
   ng1.ng_TextAttr = &ta;
   ng1.ng_VisualInfo = vis_info;
   ng1.ng_LeftEdge = win->BorderLeft + 2 * INTERWIDTH + LINEWIDTH;
   ng1.ng_TopEdge = win->BorderTop + 3 * INTERHEIGHT + LINEHEIGHT +
                    ta.ta_YSize;
   ng1.ng_Width = TextLength (&layout_rp, label, strlen (label));
   ng1.ng_Height = ta.ta_YSize;
   ng1.ng_GadgetText = label;
   ng1.ng_GadgetID = 1;
   ng1.ng_Flags = PLACETEXT_ABOVE;
   label = localized_string (MSG_NUMCOLS_GAD);
   ng2.ng_TextAttr = ng1.ng_TextAttr;
   ng2.ng_VisualInfo = ng1.ng_VisualInfo;
   ng2.ng_LeftEdge = ng1.ng_LeftEdge;
   ng2.ng_TopEdge = ng1.ng_TopEdge + ng1.ng_Height + 2 * INTERHEIGHT +
                    ta.ta_YSize;
   ng2.ng_Width = TextLength (&layout_rp, label, strlen (label));
   if (ng2.ng_Width > ng1.ng_Width)
      ng1.ng_Width = ng2.ng_Width;
   else
      ng2.ng_Width = ng1.ng_Width;
   ng2.ng_Height = ng1.ng_Height;
   ng2.ng_GadgetText = label;
   ng2.ng_GadgetID = 2;
   ng2.ng_Flags = ng1.ng_Flags;
   label = localized_string (MSG_NUMMINES_GAD);
   ng3.ng_TextAttr = ng1.ng_TextAttr;
   ng3.ng_VisualInfo = ng1.ng_VisualInfo;
   ng3.ng_LeftEdge = ng1.ng_LeftEdge;
   ng3.ng_TopEdge = ng2.ng_TopEdge + ng2.ng_Height + 2 * INTERHEIGHT +
                    ta.ta_YSize;
   ng3.ng_Width = TextLength (&layout_rp, label, strlen (label));
   if (ng3.ng_Width > ng1.ng_Width)
      ng1.ng_Width = ng2.ng_Width = ng3.ng_Width;
   else
      ng3.ng_Width = ng1.ng_Width;
   ng3.ng_Height = ng1.ng_Height;
   ng3.ng_GadgetText = label;
   ng3.ng_GadgetID = 3;
   ng3.ng_Flags = ng1.ng_Flags;
   box_w = ng1.ng_Width + 3 * INTERWIDTH + 2 * LINEWIDTH +
           TextLength (&layout_rp, "0000", 4);
   box_h = 3 * (ta.ta_YSize + ng1.ng_Height) +
           7 * INTERHEIGHT + 2 * LINEHEIGHT;
   label = localized_string (MSG_OPTIONAL_REQTITLE);
   temp = TextLength (&win->WScreen->RastPort, label, strlen (label)) + 24;
   if (temp > box_w)
   {
      box_w = temp;
      ng1.ng_Width = ng2.ng_Width = ng3.ng_Width =
                     box_w - 3 * INTERWIDTH - 2 * LINEWIDTH -
                     TextLength (&layout_rp, "0000", 4);
   }
   label = localized_string (MSG_OK_GAD);
   ng4.ng_TextAttr = win->WScreen->Font;
   ng4.ng_VisualInfo = ng1.ng_VisualInfo;
   ng4.ng_Width = TextLength (&win->WScreen->RastPort, label, strlen (label))
                  + INTERWIDTH + 2 * LINEWIDTH;
   ng4.ng_Height = win->WScreen->Font->ta_YSize +
                   INTERHEIGHT + 2 * LINEHEIGHT;
   win_w = ((box_w > ng4.ng_Width) ? box_w : ng4.ng_Width) + 2 * INTERWIDTH +
           win->BorderLeft + win->BorderRight;
   win_h = box_h + ng4.ng_Height + 3 * INTERHEIGHT +
           win->BorderTop + win->BorderBottom;
   ng4.ng_LeftEdge = (win_w - ng4.ng_Width) / 2;
   ng4.ng_TopEdge = win->BorderTop + 2 * INTERHEIGHT + box_h;
   ng4.ng_GadgetText = label;
   ng4.ng_GadgetID = 0;
   ng4.ng_Flags = 0;
   
   slider1 = CreateContext (&gad_list);
   slider1 = CreateGadget (SLIDER_KIND, slider1, &ng1,
                           GTSL_Min, MIN_ROWS, GTSL_Max, max_rows,
                           GTSL_Level, levels[OPTIONAL_LEVEL].rows,
                           GTSL_LevelFormat, "%4ld",
                           GTSL_MaxLevelLen, 4,
                           GTSL_LevelPlace, PLACETEXT_RIGHT,
                           TAG_DONE);
   slider2 = CreateGadget (SLIDER_KIND, slider1, &ng2,
                           GTSL_Min, MIN_COLUMNS, GTSL_Max, max_columns,
                           GTSL_Level, levels[OPTIONAL_LEVEL].columns,
                           GTSL_LevelFormat, "%4ld",
                           GTSL_MaxLevelLen, 4,
                           GTSL_LevelPlace, PLACETEXT_RIGHT,
                           TAG_DONE);
   max_mines = levels[OPTIONAL_LEVEL].rows * levels[OPTIONAL_LEVEL].columns *
               0.9;
   slider3 = CreateGadget (SLIDER_KIND, slider2, &ng3,
                           GTSL_Min, MIN_MINES, GTSL_Max, max_mines,
                           GTSL_Level, levels[OPTIONAL_LEVEL].bombs,
                           GTSL_LevelFormat, "%4ld",
                           GTSL_MaxLevelLen, 4,
                           GTSL_LevelPlace, PLACETEXT_RIGHT,
                           TAG_DONE);
   ok_gad = CreateGadget (BUTTON_KIND, slider3, &ng4, TAG_DONE);
   if (ok_gad)
   {
      req_win = OpenWindowTags (NULL,
                                WA_Left, win->LeftEdge +
                                         (win->Width - win_w) / 2,
                                WA_Top, win->TopEdge +
                                        (win->Height - win_h) / 2,
                                WA_Width, win_w,
                                WA_Height, win_h,
                                WA_Title,
                                     localized_string (MSG_OPTIONAL_REQTITLE),
                                WA_ScreenTitle, win->ScreenTitle,
                                WA_PubScreen, win->WScreen,
                                WA_IDCMP, BUTTONIDCMP | SLIDERIDCMP |
                                          IDCMP_REFRESHWINDOW,
                                WA_DragBar, TRUE,
                                WA_DepthGadget, TRUE,
                                WA_Activate, TRUE,
                                TAG_DONE);
      if (req_win)
      {
         struct IntuiMessage  *msg;
         struct Gadget        *gad;
         float                 mine_share;
         
         
         win_sleep = window_sleep (win, &req);
         SetAPen (req_win->RPort, gui_pens[SHINEPEN]);
         SetAfPt (req_win->RPort, dither_data, 1);
         RectFill (req_win->RPort, req_win->BorderLeft, req_win->BorderTop,
                   req_win->Width - req_win->BorderRight - 1,
                   req_win->Height - req_win->BorderBottom - 1);
         SetAPen (req_win->RPort, gui_pens[BACKGROUNDPEN]);
         SetAfPt (req_win->RPort, NULL, 0);
         RectFill (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                   req_win->BorderTop + INTERHEIGHT,
                   req_win->BorderLeft + INTERWIDTH + box_w - 1,
                   req_win->BorderTop + INTERHEIGHT + box_h - 1);
         DrawBevelBox (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                       req_win->BorderTop + INTERHEIGHT, box_w, box_h,
                       GT_VisualInfo, vis_info, GTBB_Recessed, TRUE,
                       TAG_DONE);
         AddGList (req_win, gad_list, -1, -1, NULL);
         RefreshGList (gad_list, req_win, NULL, -1);
         GT_RefreshWindow (req_win, NULL);
         
         mine_share = (float)levels[OPTIONAL_LEVEL].bombs /
                      levels[OPTIONAL_LEVEL].rows /
                      levels[OPTIONAL_LEVEL].columns;
         while (!done)
         {
            WaitPort (req_win->UserPort);
            while ((msg = GT_GetIMsg (req_win->UserPort)))
            {
               switch (msg->Class)
               {
               case IDCMP_GADGETUP:
                  done = TRUE;
                  break;
               case IDCMP_MOUSEMOVE:
                  gad = (struct Gadget *)msg->IAddress;
                  if (gad == slider1)
                  {
                     levels[OPTIONAL_LEVEL].rows = msg->Code;
                     max_mines = levels[OPTIONAL_LEVEL].rows *
                                 levels[OPTIONAL_LEVEL].columns * 0.9;
                     levels[OPTIONAL_LEVEL].bombs = mine_share *
                                               levels[OPTIONAL_LEVEL].rows *
                                               levels[OPTIONAL_LEVEL].columns;
                     if (levels[OPTIONAL_LEVEL].bombs > max_mines)
                        levels[OPTIONAL_LEVEL].bombs = max_mines;
                     else if (levels[OPTIONAL_LEVEL].bombs < MIN_MINES)
                        levels[OPTIONAL_LEVEL].bombs = MIN_MINES;
                     GT_SetGadgetAttrs (slider3, req_win, NULL,
                                     GTSL_Max, max_mines,
                                     GTSL_Level, levels[OPTIONAL_LEVEL].bombs,
                                     TAG_DONE);
                  }
                  else if (gad == slider2)
                  {
                     levels[OPTIONAL_LEVEL].columns = msg->Code;
                     max_mines = levels[OPTIONAL_LEVEL].rows *
                                 levels[OPTIONAL_LEVEL].columns * 0.9;
                     levels[OPTIONAL_LEVEL].bombs = mine_share *
                                               levels[OPTIONAL_LEVEL].rows *
                                               levels[OPTIONAL_LEVEL].columns;
                     if (levels[OPTIONAL_LEVEL].bombs > max_mines)
                        levels[OPTIONAL_LEVEL].bombs = max_mines;
                     else if (levels[OPTIONAL_LEVEL].bombs < MIN_MINES)
                        levels[OPTIONAL_LEVEL].bombs = MIN_MINES;
                     GT_SetGadgetAttrs (slider3, req_win, NULL,
                                     GTSL_Max, max_mines,
                                     GTSL_Level, levels[OPTIONAL_LEVEL].bombs,
                                     TAG_DONE);
                  }
                  else if (gad == slider3)
                     levels[OPTIONAL_LEVEL].bombs = msg->Code;
                  
                  mine_share = (float)levels[OPTIONAL_LEVEL].bombs /
                               levels[OPTIONAL_LEVEL].rows /
                               levels[OPTIONAL_LEVEL].columns;
                  break;
               case IDCMP_REFRESHWINDOW:
                  GT_BeginRefresh (req_win);
                  GT_EndRefresh (req_win, TRUE);
                  break;
               }
               GT_ReplyIMsg (msg);
            }
         }
         if (win_sleep)
            window_wakeup (win, &req);
         CloseWindow (req_win);
      }
      FreeGadgets (gad_list);
   }
}


void
request_autoopening (
   struct Window  *win)
{
   struct RastPort   layout_rp;
   struct TextAttr   ta;
   ULONG             box_w, box_h, win_w, win_h, temp;
   STRPTR            label;
   
   struct NewGadget    ng1, ng2;
   struct Gadget      *gad_list, *slider, *ok_gad;
   struct Window      *req_win;
   struct Requester    req;
   BOOL                win_sleep, done = FALSE;
   
   
   InitRastPort (&layout_rp);
   ta.ta_Name = GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
   ta.ta_YSize = GfxBase->DefaultFont->tf_YSize;
   ta.ta_Style = GfxBase->DefaultFont->tf_Style;
   ta.ta_Flags = GfxBase->DefaultFont->tf_Flags;
   
   ng1.ng_TextAttr = &ta;
   ng1.ng_VisualInfo = vis_info;
   ng1.ng_LeftEdge = win->BorderLeft + 2 * INTERWIDTH + LINEWIDTH;
   ng1.ng_TopEdge = win->BorderTop + 2 * INTERHEIGHT + LINEHEIGHT;
   ng1.ng_Width = (MAX_OPENING - MIN_OPENING + 1) * INTERWIDTH;
   ng1.ng_Height = ta.ta_YSize;
   ng1.ng_GadgetText = NULL;
   ng1.ng_GadgetID = 1;
   ng1.ng_Flags = 0;
   box_w = ng1.ng_Width + 3 * INTERWIDTH + 2 * LINEWIDTH +
           TextLength (&layout_rp, "00%", 3);
   box_h = ta.ta_YSize + 2 * (INTERHEIGHT + LINEHEIGHT);
   label = localized_string (MSG_AUTOOPEN_REQTITLE);
   temp = TextLength (&win->WScreen->RastPort, label, strlen (label)) + 24;
   if (temp > box_w)
   {
      box_w = temp;
      ng1.ng_Width = box_w - 3 * INTERWIDTH - 2 * LINEWIDTH -
                     TextLength (&layout_rp, "00%", 3);
   }
   label = localized_string (MSG_OK_GAD);
   ng2.ng_TextAttr = win->WScreen->Font;
   ng2.ng_VisualInfo = ng1.ng_VisualInfo;
   ng2.ng_Width = TextLength (&win->WScreen->RastPort, label, strlen (label))
                  + INTERWIDTH + 2 * LINEWIDTH;
   ng2.ng_Height = win->WScreen->Font->ta_YSize +
                   INTERHEIGHT + 2 * LINEHEIGHT;
   win_w = ((box_w > ng2.ng_Width) ? box_w : ng2.ng_Width) + 2 * INTERWIDTH +
           win->BorderLeft + win->BorderRight;
   win_h = box_h + ng2.ng_Height + 3 * INTERHEIGHT +
           win->BorderTop + win->BorderBottom;
   ng2.ng_LeftEdge = (win_w - ng2.ng_Width) / 2;
   ng2.ng_TopEdge = win->BorderTop + 2 * INTERHEIGHT + box_h;
   ng2.ng_GadgetText = label;
   ng2.ng_GadgetID = 0;
   ng2.ng_Flags = 0;
   
   slider = CreateContext (&gad_list);
   slider = CreateGadget (SLIDER_KIND, slider, &ng1,
                          GTSL_Min, MIN_OPENING, GTSL_Max, MAX_OPENING,
                          GTSL_Level, auto_opening,
                          GTSL_LevelFormat, "%2ld%%",
                          GTSL_MaxLevelLen, 3,
                          GTSL_LevelPlace, PLACETEXT_RIGHT,
                          TAG_DONE);
   ok_gad = CreateGadget (BUTTON_KIND, slider, &ng2, TAG_DONE);
   
   if (ok_gad)
   {
      req_win = OpenWindowTags (NULL,
                                WA_Left, win->LeftEdge +
                                         (win->Width - win_w) / 2,
                                WA_Top, win->TopEdge +
                                        (win->Height - win_h) / 2,
                                WA_Width, win_w,
                                WA_Height, win_h,
                                WA_Title,
                                     localized_string (MSG_AUTOOPEN_REQTITLE),
                                WA_ScreenTitle, win->ScreenTitle,
                                WA_PubScreen, win->WScreen,
                                WA_IDCMP, BUTTONIDCMP | SLIDERIDCMP |
                                          IDCMP_REFRESHWINDOW,
                                WA_DragBar, TRUE,
                                WA_DepthGadget, TRUE,
                                WA_Activate, TRUE,
                                TAG_DONE);
      if (req_win)
      {
         ULONG   winsig, timersig, sigmask;
         struct IntuiMessage  *msg;
         
         
         win_sleep = window_sleep (win, &req);
         SetAPen (req_win->RPort, gui_pens[SHINEPEN]);
         SetAfPt (req_win->RPort, dither_data, 1);
         RectFill (req_win->RPort, req_win->BorderLeft, req_win->BorderTop,
                   req_win->Width - req_win->BorderRight - 1,
                   req_win->Height - req_win->BorderBottom - 1);
         SetAPen (req_win->RPort, gui_pens[BACKGROUNDPEN]);
         SetAfPt (req_win->RPort, NULL, 0);
         RectFill (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                   req_win->BorderTop + INTERHEIGHT,
                   req_win->BorderLeft + INTERWIDTH + box_w - 1,
                   req_win->BorderTop + INTERHEIGHT + box_h - 1);
         DrawBevelBox (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                       req_win->BorderTop + INTERHEIGHT, box_w, box_h,
                       GT_VisualInfo, vis_info, GTBB_Recessed, TRUE,
                       TAG_DONE);
         AddGList (req_win, gad_list, -1, -1, NULL);
         RefreshGList (gad_list, req_win, NULL, -1);
         GT_RefreshWindow (req_win, NULL);
         
         winsig = 1L << req_win->UserPort->mp_SigBit;
         timersig = timer_signal (timer_obj);
         while (!done)
         {
            sigmask = Wait (winsig | timersig);
            if (sigmask & winsig)
            {
               while (NULL !=(msg = GT_GetIMsg (req_win->UserPort)))
               {
                  switch (msg->Class)
                  {
                  case IDCMP_GADGETUP:
                     done = TRUE;
                     break;
                  case IDCMP_MOUSEMOVE:
                     auto_opening = msg->Code;
                     break;
                  case IDCMP_REFRESHWINDOW:
                     GT_BeginRefresh (req_win);
                     GT_EndRefresh (req_win, TRUE);
                     break;
                  }
                  GT_ReplyIMsg (msg);
               }
            }
            if (sigmask & timersig)
            {
               timer_reply (timer_obj);
               if (time_on)
               {
                  timer_start (timer_obj, 0L, 1000000L);
                  counter_update (time_counter,
                                  counter_value (time_counter) + 1);
               }
            }
         }
         if (win_sleep)
            window_wakeup (win, &req);
         CloseWindow (req_win);
      }
      FreeGadgets (gad_list);
   }
}
