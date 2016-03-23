#include <stdio.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <exec/types.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include "chunkyiclass.h"


BOOL initialize (void);
void finalize (void);
void event_loop (void);


struct Library  *IntuitionBase;

struct Window    *win;
struct DrawInfo  *dri;
Class            *cl;
struct Image     *img;


UBYTE   calm_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,2,2,2,2,2,2,1,1,0,0,0,
   0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,
   0,1,2,2,2,2,2,2,2,2,2,2,2,2,1,0,
   0,1,2,2,2,1,1,2,2,1,1,2,2,2,1,0,
   1,2,2,2,2,1,1,2,2,1,1,2,2,2,2,1,
   1,2,2,2,2,1,1,2,2,1,1,2,2,2,2,1,
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
   0,1,2,2,2,1,2,2,2,2,1,2,2,2,1,0,
   0,1,2,2,2,2,1,1,1,1,2,2,2,2,1,0,
   0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,
   0,0,0,1,1,2,2,2,2,2,2,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

UBYTE   scared_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,2,2,2,2,2,2,1,1,0,0,0,
   0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,
   0,1,2,2,2,1,1,2,2,1,1,2,2,2,1,0,
   0,1,2,2,1,2,2,2,2,2,2,1,2,2,1,0,
   1,2,2,2,2,1,1,2,2,1,1,2,2,2,2,1,
   1,2,2,2,2,1,1,2,2,1,1,2,2,2,2,1,
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
   0,1,2,2,2,2,1,1,1,1,2,2,2,2,1,0,
   0,1,2,2,2,1,1,1,1,1,1,2,2,2,1,0,
   0,0,1,2,2,2,2,2,2,2,2,2,2,1,0,0,
   0,0,0,1,1,2,2,2,2,2,2,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

ULONG   img_palette[] = {
   2,
   0x00000000, 0x00000000, 0x00000000,
   0xEEEEEEEE, 0xDDDDDDDD, 0x00000000
};


int
main (void)
{
   if (initialize ())
      event_loop ();
   finalize ();
   return 0;
}

void
event_loop (void)
{
   struct IntuiMessage  *msg;
   BOOL   done = FALSE;
   
   while (!done)
   {
      WaitPort (win->UserPort);
      while (msg = (struct IntuiMessage *)GetMsg (win->UserPort))
      {
         if (msg->Class == IDCMP_CLOSEWINDOW)
            done = TRUE;
         ReplyMsg ((struct Message *)msg);
      }
   }
}

BOOL
initialize (void)
{
   if (IntuitionBase = OpenLibrary ("intuition.library", 37L))
   {
      win = OpenWindowTags (NULL,
                            WA_Width, 72,
                            WA_Height, 54,
                            WA_Title, "chynkyiclass",
                            WA_Activate, TRUE,
                            WA_DragBar, TRUE,
                            WA_DepthGadget, TRUE,
                            WA_CloseGadget, TRUE,
                            WA_IDCMP, IDCMP_CLOSEWINDOW,
                            TAG_DONE);
      if (win)
      {
         dri = GetScreenDrawInfo (win->WScreen);
         if (cl = init_chunkyiclass ())
         {
            img = (struct Image *)NewObject (cl, NULL,
                                             IA_Data, calm_data,
                                             CHUNKYIA_SelectedData, scared_data,
                                             CHUNKYIA_Palette, img_palette,
                                             CHUNKYIA_Screen, win->WScreen,
                                             TAG_DONE);
            if (img)
            {
               DrawImageState (win->RPort, img, 20, 20, IDS_NORMAL, NULL);
               DrawImageState (win->RPort, img, 36, 20, IDS_SELECTED, NULL);
               
               return TRUE;
            }
         }
      }
   }
   
   return FALSE;
}

void
finalize (void)
{
   if (img)
      DisposeObject (img);
   if (cl)
      free_chunkyiclass (cl);
   if (dri)
      FreeScreenDrawInfo (win->WScreen, dri);
   if (win)
      CloseWindow (win);
   if (IntuitionBase)
      CloseLibrary (IntuitionBase);
}
