/*
 *
 * cgfx.c
 *
 */

#include <stdlib.h>
#include "cgfx.h"
#include "../main/main.h"
#include "../main/prefs.h"

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

#include <powerup/ppcinline/cybergraphics.h>
#else

#include "aros-inc.h"

#endif

#include <cybergraphx/cybergraphics.h>

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *CyberGfxBase;
extern struct Screen *screen;
extern struct ScreenBuffer *sbuffer[3];

/* CGFX variables */
unsigned char *baseaddress[3] = { NULL, NULL, NULL };
unsigned long pixfmt = 0, bpr = 0, bpp = 1;

void close_screen_cgfx()
{
  if (prefs.triple == PREFS_ON) {
    if (sbuffer[0] != NULL) {
      FreeScreenBuffer(screen, sbuffer[0]);
      sbuffer[0] = NULL;
    }
    if (sbuffer[1] != NULL) {
      FreeScreenBuffer(screen, sbuffer[1]);
      sbuffer[1] = NULL;
    }
    if (sbuffer[2] != NULL) {
      FreeScreenBuffer(screen, sbuffer[2]);
      sbuffer[2] = NULL;
    }
  }

  if (screen != NULL) {
    CloseScreen(screen);
    screen = NULL;
  }
}

int open_screen_cgfx(int width, int height, unsigned long id)
{
  unsigned long rgbtab[1+3*256+1];
  int i, depth;

  /* Get VideoMode information */
  depth = GetCyberIDAttr(CYBRIDATTR_DEPTH, id);

  /* Open screen */
  screen = OpenScreenTags(NULL,
               SA_Width,width,
               SA_Height,height,
               SA_Depth,depth,
               SA_Quiet,TRUE,
               SA_ShowTitle,FALSE,
               SA_Type,CUSTOMSCREEN,
               SA_DisplayID,id,
               TAG_DONE);

  if(screen == NULL) {
    amp_printf("could not open the screen\n");
    return -1;
  }

  if (prefs.triple == PREFS_ON) {
    sbuffer[0] = AllocScreenBuffer(screen, NULL, SB_SCREEN_BITMAP);
    if (sbuffer[0] == NULL) {
      amp_printf("screenbuffer 1 allocation failed\n");
      return -1;
    }
    sbuffer[1] = AllocScreenBuffer(screen, NULL, 0);
    if (sbuffer[1] == NULL) {
      amp_printf("screenbuffer 2 allocation failed\n");
      return -1;
    }
    sbuffer[2] = AllocScreenBuffer(screen, NULL, 0);
    if (sbuffer[2] == NULL) {
      amp_printf("screenbuffer 3 allocation failed\n");
      return -1;
    }
  }

  if (prefs.triple == PREFS_ON) {
    for (i=0; i<3; i++) {
      /* Get screenmode information */
      UnLockBitMap(LockBitMapTags(sbuffer[i]->sb_BitMap,
                                  LBMI_PIXFMT, (ULONG)&pixfmt,
                                  LBMI_BASEADDRESS, (ULONG)&baseaddress[i],
                                  LBMI_BYTESPERROW, (ULONG)&bpr,
                                  LBMI_BYTESPERPIX, (ULONG)&bpp,
                                  TAG_END));

      memset(baseaddress[i], 0, (height*bpr));
    }
  } else {
    /* Get screenmode information */
    UnLockBitMap(LockBitMapTags(screen->RastPort.BitMap,
                                LBMI_PIXFMT, (ULONG)&pixfmt,
                                LBMI_BASEADDRESS, (ULONG)&baseaddress[0],
                                LBMI_BYTESPERROW, (ULONG)&bpr,
                                LBMI_BYTESPERPIX, (ULONG)&bpp,
                                TAG_END));

    memset(baseaddress[0], 0, (height*bpr));
  }

  /* Set ALL colors to black */
  if (depth == 8) {
    for(i=0; i<256; i++) {
      rgbtab[1+i*3+0] = 0;
      rgbtab[1+i*3+1] = 0;
      rgbtab[1+i*3+2] = 0;
    }

    rgbtab[0] = (256<<16)|0;
    rgbtab[1+256*3] = 0;
    LoadRGB32(&screen->ViewPort, rgbtab);
  }

  return 0;
}
