/*
 *
 * req.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#if !defined(__AROS__)
#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/graphics.h>
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcproto/asl.h>
#include <powerup/ppcproto/gadtools.h>
#else

#include "aros-inc.h"

#endif
#include <graphics/displayinfo.h>

#include "../main/main.h"
#include "../main/prefs.h"
#include "req.h"

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

#define AMP_FLAGS (WFLG_NOCAREREFRESH|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_RMBTRAP|WFLG_GIMMEZEROZERO)
#define AMP_IDCMP (IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_GADGETUP|IDCMP_GADGETDOWN)

static char req_str[10][100][32], *req_str_ptr[10][100], req_header[10][32], req_title[32];
static int req_ptr[10], req_res[10], req_def[10], req_max;

void req_init(char *title)
{
  memset(req_str, 0, sizeof(req_str));
  memset(req_ptr, 0, sizeof(req_ptr));
  memset(req_res, 0, sizeof(req_res));
  memset(req_str_ptr, 0, sizeof(req_str_ptr));
  memset(req_header, 0, sizeof(req_header));
  memset(req_title, 0, sizeof(req_title));
  memset(req_def, 0, sizeof(req_def));

  strncpy(req_title, title, 31);

  req_max = 0;
}

void req_name(int item, char *str)
{
  strncpy(req_header[item], str, 31);

  if (item > req_max) {
    req_max = item;
  }
}

void req_add(int item, char *str)
{
  int nr = req_ptr[item];

  req_str_ptr[item][nr] = req_str[item][nr];
  strncpy(req_str[item][nr], str, 31);

  req_ptr[item]++;
}

void req_default(int item, int nr)
{
  req_def[item] = nr;
}

/* This is really silly, but this variable has to be placed
 * here, if it's placed inside the req_show function, one will
 * get a PPC Alignment Exception.
 */
static struct TextAttr topaz8 = {"topaz.font", 8, 0x00, 0x01};

void req_show(void)
{
  struct Gadget *gl = NULL, *g[11];
  struct IntuiMessage *imsg = NULL;
  struct Screen *screen = NULL;
  struct Window *window = NULL;
  struct NewGadget ng[11];
  ULONG imCode, imClass;
  APTR VisualInfo;
  int i;

  screen = LockPubScreen(NULL);
  VisualInfo = GetVisualInfo(screen);

  for (i=0; i<=(req_max + 1); i++) {
    ng[i].ng_LeftEdge = 10 + (12 * 8);
    ng[i].ng_TopEdge = 2 + (15 * i);
    ng[i].ng_Width = 284;
    ng[i].ng_Height = 13;
    ng[i].ng_GadgetText = req_header[i];
    ng[i].ng_TextAttr = &topaz8;
    ng[i].ng_GadgetID = i;
    ng[i].ng_Flags = 0;
    ng[i].ng_VisualInfo = VisualInfo;
    ng[i].ng_UserData = NULL;
  }

  ng[i-1].ng_Flags = PLACETEXT_IN; /* OK button */
  strcpy(req_header[i-1], "OK");

  g[0] = CreateContext(&gl);
  for (i=0; i<=req_max; i++) {
    g[i+1] = CreateGadget(CYCLE_KIND, g[i], &ng[i], GTCY_Labels, req_str_ptr[i], GTCY_Active, req_def[i], TAG_DONE);
  }
  g[i+1] = CreateGadget(BUTTON_KIND, g[i], &ng[i], TAG_DONE);

  window = OpenWindowTags(NULL,
                          WA_Title, req_title,
                          WA_ScreenTitle, prefs.registered,
                          WA_Flags, AMP_FLAGS & (~WFLG_CLOSEGADGET),
                          WA_IDCMP, AMP_IDCMP,
                          WA_Gadgets, gl,
                          WA_Left, 100,
                          WA_Top, 100,
                          WA_Width, 406,
                          WA_Height, 5 + (15 * (req_max + 3)),
                          WA_Activate, TRUE,
                          TAG_DONE);

  if (window == NULL) {
    amp_printf("could NOT open requester window\n");
  } else {
    GT_RefreshWindow(window, NULL);

    for(;;) {
      int gad_num = -1;
      WaitPort(window->UserPort);

      if ((imsg = (struct IntuiMessage *)GT_GetIMsg(window->UserPort))) {
        imClass = imsg->Class;
        imCode = imsg->Code;

        if (imClass == IDCMP_GADGETUP) {
          gad_num = ((struct Gadget *)imsg->IAddress)->GadgetID;
        }

        GT_ReplyIMsg(imsg);

        if ((imClass == IDCMP_GADGETUP) && (gad_num == (req_max + 1))) {
          break;
        }

        if (imClass == IDCMP_REFRESHWINDOW) {
           GT_BeginRefresh(window);
          GT_EndRefresh(window, TRUE);
        }
      }
    }

    for (i=0; i<=req_max; i++) {
      GT_GetGadgetAttrs(g[i+1], window, NULL, GTCY_Active, &req_res[i], TAG_DONE);
    }

    CloseWindow(window);
  }

  FreeGadgets(gl);

  FreeVisualInfo(VisualInfo);
  UnlockPubScreen(NULL, screen);
}

int req_get(int item)
{
  return req_res[item];
}

void req_exit(void)
{
}
