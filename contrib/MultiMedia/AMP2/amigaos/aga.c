/*
 *
 * aga.c
 *
 */

#include <stdlib.h>
#include "aga.h"
#include "video.h"
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
#else

#include "aros-inc.h"

#endif

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Screen *screen;
extern struct ScreenBuffer *sbuffer[3];

struct BitMap agabitmap[3];

static int aga_width, aga_height, aga_depth;
static unsigned char *aga_bitplanes = NULL;

void close_screen_aga()
{
  int b;

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

  if (prefs.triple == PREFS_ON) {
    b = 3;
  } else {
    b = 1;
  }

  if (aga_bitplanes != NULL) {
    FreeRaster(aga_bitplanes, aga_width, aga_height*aga_depth*b);
    aga_bitplanes = NULL;
  }
}

unsigned long get_screenmode_aga(int width, int height, int depth, unsigned long id)
{
  aga_width = width;
  aga_height = height;
  aga_depth = depth;

  /* Make changes if HAM is selected */
  if (prefs.colormode == PREFS_HAM) {
    aga_depth = prefs.ham_depth;
    if (prefs.screenmode == PREFS_PAL) {
      if (prefs.ham_width == 4) {
        aga_width = 1280;
        aga_height = 256;
        id = 0x00029820; /* SuperHiresHAM */
      } else if (prefs.ham_width == 2) {
        if (prefs.half == PREFS_ON) {
          id = 0x00029808; /* SuperLoresHiresHAM */
          aga_width = 640;
          aga_height = 128;
        } else {
          id = 0x00029800; /* HiresHAM */
          aga_width = 640;
          aga_height = 256;
        }
      } else if (prefs.ham_width == 1) {
        if (prefs.half == PREFS_ON) {
          id = 0x00021808; /* SuperLoresLoresHAM */
          aga_width = 320;
          aga_height = 128;
        } else {
          id = 0x00021800; /* LoresHAM */
          aga_width = 320;
          aga_height = 256;
        }
      }
    } else if (prefs.screenmode == PREFS_HIPAL) {
      if (prefs.ham_width == 2) {
        if (prefs.half == PREFS_ON) {
          id = 0x00029820; /* SuperHiresHAM */
          aga_width = 1280;
          aga_height = 256;
        } else {
          id = 0x00029824; /* SuperHiresLacedHAM */
          aga_width = 1280;
          aga_height = 512;
        }
      } else if (prefs.ham_width == 1) {
        if (prefs.half == PREFS_ON) {
          id = 0x00029800; /* HiresHAM */
          aga_width = 640;
          aga_height = 256;
        } else {
          id = 0x00029804; /* HiresLacedHAM */
          aga_width = 640;
          aga_height = 512;
        }
      }
    } else {
      id |= 0x800; /* HAM */
    }
  } else {
    if (prefs.screenmode == PREFS_PAL) {
      if (prefs.half == PREFS_ON) {
        id = 0x00021008; /* SuperLoresLores */
        aga_width = 320;
        aga_height = 128;
      } else {
        id = 0x00021000; /* Lores */
        aga_width = 320;
        aga_height = 256;
      }
    } else if (prefs.screenmode == PREFS_HIPAL) {
      if (prefs.half == PREFS_ON) {
        id = 0x00029000; /* Hires */
        aga_width = 640;
        aga_height = 256;
      } else {
        id = 0x00029004; /* HiresLaced */
        aga_width = 640;
        aga_height = 512;
      }
    }
  }

  /* Change depth if gray mode is selected */
  if (prefs.colormode == PREFS_GRAY) {
    aga_depth = prefs.gray_depth;
  }

  aga_width &= ~63;

  return id;
}

int open_screen_aga(int width, int height, unsigned long id)
{
  unsigned long dispid, rgbtab[1+3*256+1], free_mem;
  unsigned char *temp;
  int i, j, colors, b;

  dispid = get_screenmode_aga(width, height, 8, id);
  if (dispid == 0) {
    return -1;
  }

  /* Allocate resources */
  if (prefs.triple == PREFS_ON) {
    b = 3;
  } else {
    b = 1;
  }

  /* Always leave 64+32KB free for other tasks, audio.device uses 32*2=64KB */
  #define SAFE ((64+32)*1024)

  free_mem = AvailMem(MEMF_CHIP|MEMF_LARGEST) - SAFE;

  if (free_mem < ((aga_width*aga_height*aga_depth*b)/8)) {
    amp_printf("AGA initialization failed, not enough free CHIP memory\n");
    return -1;
  }

  aga_bitplanes = (unsigned char*)AllocRaster(aga_width, aga_height*aga_depth*b);
  memset(aga_bitplanes, 0, (aga_width*aga_height*aga_depth*b)/8);

  temp = aga_bitplanes;
  for (j=0; j<b; j++) {
    InitBitMap(&agabitmap[j], aga_depth, aga_width, aga_height); 

    for (i=0; i<aga_depth; i++) {
      agabitmap[j].Planes[i] = temp;
      temp += RASSIZE(aga_width, aga_height);
    }
  }

  /* Open screen */
  screen = OpenScreenTags(NULL,
               SA_BitMap,(int)&agabitmap[0],
               SA_Width,aga_width,
               SA_Height,aga_height,
               SA_Depth,aga_depth,
               SA_Quiet,TRUE,
               SA_ShowTitle,FALSE,
               SA_Type,CUSTOMSCREEN,
               SA_DisplayID,dispid,
               TAG_DONE);

  if (screen == NULL) {
    amp_printf("could not open the screen\n");
    return -1;
  }

  if (prefs.triple == PREFS_ON) {
    sbuffer[0] = AllocScreenBuffer(screen, &agabitmap[0], 0);
    if (sbuffer[0] == NULL) {
      amp_printf("screenbuffer 1 allocation failed\n");
      return -1;
    }
    sbuffer[1] = AllocScreenBuffer(screen, &agabitmap[1], 0);
    if (sbuffer[1] == NULL) {
      amp_printf("screenbuffer 2 allocation failed\n");
      return -1;
    }
    sbuffer[2] = AllocScreenBuffer(screen, &agabitmap[2], 0);
    if (sbuffer[2] == NULL) {
      amp_printf("screenbuffer 3 allocation failed\n");
      return -1;
    }
  }

  /* Set ALL colors to black */
  if (prefs.colormode == PREFS_HAM) {
    colors = 1 << (aga_depth - 2);
  } else {
    colors = 1 << aga_depth;
  }

  for (i=0; i<colors; i++) {
    rgbtab[1+i*3+0] = 0;
    rgbtab[1+i*3+1] = 0;
    rgbtab[1+i*3+2] = 0;
  }

  rgbtab[0] = (colors<<16)|0;
  rgbtab[1+colors*3] = 0;
  LoadRGB32(&screen->ViewPort, rgbtab);

  /* Set HAM palette if required */
  if ((prefs.ham_width == 1) && (prefs.ham_quality == PREFS_HIGH)) {
    unsigned long rgbtab[1+3*256+1];
    int i, red, green, blue;

    if (prefs.ham_depth == 8) {
      /* Set the palette (HAM8) */
      for(i=0; i<64; i++) {
        red   = (i<<2) & 0xc0;
        green = (i<<4) & 0xc0;
        blue  = (i<<6) & 0xc0;

        rgbtab[1+i*3+0] = (red<<24);
        rgbtab[1+i*3+1] = (green<<24);
        rgbtab[1+i*3+2] = (blue<<24);
      }

      rgbtab[0] = (64<<16)|0;
      rgbtab[1+64*3] = 0;
      LoadRGB32(&screen->ViewPort, rgbtab);
    } else {
      /* Set the palette (HAM6) */
      for(i=0; i<16; i++) {
        red   = (i<<4) & 0x80;
        green = (i<<5) & 0xc0;
        blue  = (i<<7) & 0x80;

        rgbtab[1+i*3+0] = (red<<24);
        rgbtab[1+i*3+1] = (green<<24);
        rgbtab[1+i*3+2] = (blue<<24);
      }

      rgbtab[0] = (16<<16)|0;
      rgbtab[1+16*3] = 0;
      LoadRGB32(&screen->ViewPort, rgbtab);
    }
  }

  return 0;
}
