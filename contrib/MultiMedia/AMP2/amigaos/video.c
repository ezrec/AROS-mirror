/*
 *
 * video.c
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
#include <powerup/ppcinline/cybergraphics.h>
#include <proto/picasso96.h>
#else

#include "aros-inc.h"

#endif

#include <cybergraphx/cgxvideo.h>
#include <proto/cgxvideo.h>
#include <cybergraphx/cybergraphics.h>

#include <graphics/displayinfo.h>

#include "video.h"
#include "aga.h"
#include "cgfx.h"
#include "../main/main.h"
#include "../main/prefs.h"
#include "../refresh/refresh.h"
#include "../refresh/refresh_internal.h"

#define AGA  1
#define CGFX 2

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *CyberGfxBase;
extern struct Library *CGXVideoBase;
extern struct Library *P96Base;
extern unsigned char *baseaddress[3];
extern unsigned long pixfmt, bpr, bpp;
extern struct BitMap agabitmap[3];

struct Screen *screen = NULL;
struct Window *window = NULL;
struct ScreenBuffer *sbuffer[3] = { NULL, NULL, NULL };

static struct ScreenModeRequester *screenreq = NULL;
static ULONG rgbtab[1+3*256+1];
static struct DimensionInfo dim;
#if !defined(__AROS__)
static VLayerHandle VLH = NULL;
#else
static struct VLayerHandle *VLH = NULL;
#endif
static UWORD *EmptyPointer = NULL;
static int cur_sbuffer = 0, free_sbuffer = 0;
static int use_vlayer = 0, screentype = 0;
static unsigned long *vlayerimage = NULL;
static struct PropInfo BotGadInfo = {0};
static struct Image BotGadImage = {0};
static struct Gadget BotGad = {0};
static unsigned long *aga_window_buffer = NULL;
static struct BitMap *window_bitmap = NULL;
static int amiga_depth = 0;
static char *window_text = NULL;
static int overlay_is_locked = 0;
static int down = 0, amiga_is_down = 0;

static char pixformats[][8] = {
"LUT8\0   ",
"RGB15\0  ",
"BGR15\0  ",
"RGB15PC\0",
"BGR15PC\0",
"RGB16\0  ",
"BGR16\0  ",
"RGB16PC\0",
"BGR16PC\0",
"RGB24\0  ",
"BGR24\0  ",
"ARGB32\0 ",
"BGRA32\0 ",
"RGBA32\0 "};

static struct {
  unsigned long cgfx_mode;
  unsigned long refresh_mode;
} mode_conv[] = {
{ PIXFMT_LUT8,    PIXEL_MODE_LUT8    },
{ PIXFMT_RGB15,   PIXEL_MODE_RGB15   },
{ PIXFMT_RGB16,   PIXEL_MODE_RGB16   },
{ PIXFMT_ARGB32,  PIXEL_MODE_ARGB32  },
{ PIXFMT_BGR24,   PIXEL_MODE_BGR24   },
{ PIXFMT_RGB15PC, PIXEL_MODE_RGB15PC },
{ PIXFMT_RGB16PC, PIXEL_MODE_RGB16PC },
{ PIXFMT_BGRA32,  PIXEL_MODE_BGRA32  },
{ 0, 0}};

#define AMP_FLAGS (WFLG_NOCAREREFRESH|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_RMBTRAP|WFLG_GIMMEZEROZERO)
#define AMP_IDCMP (IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_GADGETUP|IDCMP_GADGETDOWN)

void init_gadgets(void)
{
  BotGadInfo.Flags     = AUTOKNOB | FREEHORIZ | PROPNEWLOOK;
  BotGadInfo.HorizPot  = 0;
  BotGadInfo.VertPot   = 0;
  BotGadInfo.HorizBody = MAXBODY/64;
  BotGadInfo.VertBody  = 0;

  BotGad.LeftEdge = 3;
  BotGad.TopEdge  = -9;
  BotGad.Width    = -6;
  BotGad.Height   = 8;

  BotGad.Flags        = GFLG_RELBOTTOM | GFLG_RELWIDTH;
  BotGad.Activation   = GACT_RELVERIFY | GACT_IMMEDIATE | GACT_BOTTOMBORDER;
  BotGad.GadgetType   = GTYP_PROPGADGET | GTYP_GZZGADGET;
  BotGad.GadgetRender = (APTR)&BotGadImage;
  BotGad.SpecialInfo  = (APTR)&BotGadInfo;
  BotGad.GadgetID     = 0;
}

unsigned long get_screenmode_ID(int *width, int *height)
{
  unsigned long id = 0;

  if (prefs.screenmode == PREFS_BESTMODE) {
    /* Best Mode, use custom implementation for CGFX */
    if(CyberGfxBase != NULL) {
      unsigned short cmodels_low[] = {PIXFMT_LUT8, -1};
      unsigned short cmodels_high[] = {PIXFMT_RGB15, PIXFMT_RGB16, -1};
      unsigned short cmodels_true[] = {PIXFMT_BGR24, PIXFMT_ARGB32, -1};
      unsigned short *cmodels = NULL;
      struct List *cmodelist = NULL;
      struct CyberModeNode *mode;

      if ((prefs.cgfx_depth == PREFS_LOWCOLOR) || (prefs.colormode == PREFS_GRAY)) {
        cmodels = cmodels_low;
      } else if (prefs.cgfx_depth == PREFS_HIGHCOLOR) {
        cmodels = cmodels_high;
      } else {
        cmodels = cmodels_true;
      }

      cmodelist = AllocCModeListTags(CYBRMREQ_MinWidth, *width,
                                     CYBRMREQ_MinHeight, *height,
                                     CYBRMREQ_CModelArray, cmodels,
                                     TAG_DONE);

      if (cmodelist != NULL) {
        if (!IsListEmpty(cmodelist)) {
          int depth = 0;
          *width = 16384; *height = 16384; /* Impossible values */
          mode = (struct CyberModeNode *)cmodelist->lh_Head;
          while (mode->Node.ln_Succ != NULL) {
/*
            debug_printf("MODE: %d x %d x %d, %08lx\n", mode->Width, mode->Height, mode->Depth, mode->DisplayID);
*/
            /* If the size of the previous best match is the same as the current, only swap
               if we'll get a 24/16bit mode instead of a 32/15bit one */
            if ((mode->Width == *width) && (mode->Height == *height)) {
              if ((mode->Depth == 16) || (mode->Depth == 24)) {
                depth = mode->Depth;
                id = mode->DisplayID;
              }
            } else {
              /* Choose the smallest suitable screenmode */
              if ((mode->Width <= *width) || (mode->Height <= *height)) {
                *width = mode->Width;
                *height = mode->Height;
                depth = mode->Depth;
                id = mode->DisplayID;
              }
            }
            mode = (struct CyberModeNode *)mode->Node.ln_Succ;
          }
        }
        FreeCModeList(cmodelist);
      }

      if (id == 0) {
        amp_printf("no suitable mode was found, try using the requester\n");
      }
    } else {
      id = BestModeID(BIDTAG_DesiredWidth, *width,
                      BIDTAG_DesiredHeight, *height,
                      BIDTAG_NominalWidth, *width,
                      BIDTAG_NominalHeight, *height,
                      BIDTAG_Depth, 8);
      if (id != INVALID_ID) {
        if (GetDisplayInfoData(NULL, &dim, sizeof(dim), DTAG_DIMS, id) == 0) {
          id = 0;
        }
        *width = dim.Nominal.MaxX + 1;
        *height = dim.Nominal.MaxY + 1;
      } else {
        id = 0;
      }
    }
  } else if (prefs.screenmode == PREFS_REQUESTER) {
    /* Requester */
    char asl_title[32];
    sprintf(asl_title, "%dx%d", *width, *height);
    screenreq = AllocAslRequestTags(ASL_ScreenModeRequest, TAG_END);
    if (screenreq != NULL) {
      int asl_done = 0;
      if (AslRequestTags(screenreq, ASLSM_TitleText, asl_title, TAG_END)) {
        id = screenreq->sm_DisplayID;

        if (CyberGfxBase != NULL) {
          if (IsCyberModeID(id)) {
            *width = GetCyberIDAttr(CYBRIDATTR_WIDTH, id);
            *height = GetCyberIDAttr(CYBRIDATTR_HEIGHT, id);
            asl_done = 1;
          }
        }

        if (asl_done == 0) {
          if (GetDisplayInfoData(NULL, &dim, sizeof(dim), DTAG_DIMS, id) == 0) {
            id = 0;
          } else {
            *width = dim.Nominal.MaxX + 1;
            *height = dim.Nominal.MaxY + 1;
          }
        }
      } else {
        id = 0;
      }
      FreeAslRequest(screenreq);
    } else {
      id = 0;
    }
  } else if (prefs.screenmode == PREFS_HIPAL) {
    /* PAL HiresLaced */
    *width = 640;
    *height = 512;
    id = 0x00029004;
  } else {
    /* PAL Lores */
    *width = 320;
    *height = 256;
    id = 0x00021000;
  }

  return id;
}

/* Free resources allocated by open_screen or open_audio_window */
void close_screen()
{
  if (amiga_depth == 8) {
    if (prefs.window == PREFS_ON) {
      int i;

      for (i=0; i<256; i++) {
        ReleasePen(window->WScreen->ViewPort.ColorMap, refresh_rect.pixel[i]);
      }
    }
  }

  if (VLH != NULL) {
    DetachVLayer(VLH);
    DeleteVLayerHandle(VLH);
    VLH = NULL;
  }

  if (window != NULL) {
    CloseWindow(window);
    window = NULL;
  }

  if (screen != NULL) {
    if (screentype == AGA) {
      close_screen_aga();
    } else if (screentype == CGFX) {
      close_screen_cgfx();
    }
  }

  if (EmptyPointer != NULL) {
    FreeVec(EmptyPointer);
    EmptyPointer = NULL;
  }

  if (window_bitmap != NULL) {
    FreeBitMap(window_bitmap);
    window_bitmap = NULL;
  }

  if (refresh_rect.pixel != NULL) {
    free(refresh_rect.pixel);
    refresh_rect.pixel = NULL;
  }
}

/* Triple buffering, call first, then render to free_buffer */
static void triple_refresh_init(void)
{
  /* Next buffer please */
  cur_sbuffer++;
  if (cur_sbuffer > 2) {
    cur_sbuffer = 0;
  }

  /* Display the next buffer */
  sbuffer[cur_sbuffer]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
  while (!ChangeScreenBuffer(screen, sbuffer[cur_sbuffer]));

  /* Always render to the buffer to be displayed next since touching the
     previous or current is a "no no" since I don't check when the buffer
     actually has been switched, so this is the only safe way */

  free_sbuffer = cur_sbuffer + 1;
  if (free_sbuffer > 2) {
    free_sbuffer = 0;
  }

  if (screentype == AGA) {
    refresh_rect.dst = (unsigned long *)agabitmap[free_sbuffer].Planes[0];
  } else {
    refresh_rect.dst = (unsigned long *)baseaddress[free_sbuffer];
  }
}

/*
static void triple_refresh_exit(void)
{
}
*/

/*
static void window_refresh_init()
{
}
*/

static void window_refresh_exit()
{
  BltBitMapRastPort(window_bitmap, 0, 0, window->RPort, 0, 0, refresh_rect.width, refresh_rect.height, 0xc0);
}

static void overlay_refresh_init()
{
  if (LockVLayer(VLH)) {
    vlayerimage = (unsigned long *)GetVLayerAttr(VLH, VOA_BaseAddress);
    refresh_rect.dst = vlayerimage;
    overlay_is_locked = 1;
  } else {
    overlay_is_locked = 0;
  }
}

static void overlay_refresh_exit()
{
  if (overlay_is_locked == 1) {
    UnlockVLayer(VLH);
  }
}

/*
static void aga_window_refresh_init()
{
}
*/

static void aga_window_refresh_exit()
{
  WriteChunkyPixels(window->RPort, 0, 0, refresh_rect.width, refresh_rect.height, refresh_rect.dst, refresh_rect.width);
}

/* Returns "depth", i.e. 8 or 32 which are the only supported ones or -1 on error */

int open_screen(int width, int height, int mode)
{
  int screen_width, screen_height;
  unsigned long error, dispid;
  unsigned long my_depth = 0;
  int i, b;

  refresh_rect.pixel = malloc(256);

  screen_width = width;
  screen_height = height;

  cur_sbuffer = 0;
  free_sbuffer = 0;
  use_vlayer = 0;
  screentype = 0;
  down = 0;
  amiga_is_down = 0;

  init_gadgets();

  /* Disable triple buffering when not used anyway */
  if ((prefs.window == PREFS_ON) || (prefs.overlay == PREFS_ON)) {
    prefs.triple = PREFS_OFF;
  }

  if (prefs.window == PREFS_ON) {
    /* HAM not supported in window mode */
    if (prefs.colormode == PREFS_HAM) {
      prefs.colormode = PREFS_COLOR;
    }

    if ((CGXVideoBase != NULL) && (prefs.overlay == PREFS_ON)) {
      BotGad.Width = -23;
      window = OpenWindowTags(NULL,
                              WA_Title, prefs.window_title,
                              WA_ScreenTitle, prefs.registered,
                              WA_Flags, AMP_FLAGS|WFLG_SIZEGADGET|WFLG_SIZEBBOTTOM,
                              WA_IDCMP, AMP_IDCMP,
                              WA_Gadgets, &BotGad,
                              WA_Left, 100,
                              WA_Top, 100,
                              WA_InnerWidth, width,
                              WA_InnerHeight, height,
                              WA_MaxWidth, width*24,
                              WA_MaxHeight, height*24,
                              WA_Activate, TRUE,
                              TAG_DONE);
    } else {
      BotGad.Width = -6;
      window = OpenWindowTags(NULL,
                              WA_Title, prefs.window_title,
                              WA_ScreenTitle, prefs.registered,
                              WA_Flags, AMP_FLAGS,
                              WA_IDCMP, AMP_IDCMP,
                              WA_Gadgets, &BotGad,
                              WA_Left, 100,
                              WA_Top, 100,
                              WA_InnerWidth, width,
                              WA_InnerHeight, height,
                              WA_Activate, TRUE,
                              TAG_DONE);
    }

    if (window == NULL) {
      return -1;
    }

    if (GetCyberMapAttr(window->RPort->BitMap, CYBRMATTR_ISCYBERGFX)) {
      screentype = CGFX;
    } else {
      screentype = AGA;
    }

    if ((CyberGfxBase != NULL) && (screentype == CGFX)) {
      UnLockBitMap(LockBitMapTags(window->RPort->BitMap,
                                  LBMI_PIXFMT, (ULONG)&pixfmt,
                                  LBMI_BASEADDRESS, (ULONG)&baseaddress[free_sbuffer],
                                  LBMI_BYTESPERPIX, (ULONG)&bpp,
                                  LBMI_BYTESPERROW, (ULONG)&bpr,
                                  LBMI_DEPTH, (ULONG)&my_depth,
                                  TAG_END));

      if (pixfmt == PIXFMT_LUT8) {
        amiga_depth = 8;
      } else {
        amiga_depth = 32;
      }
    } else {
      amiga_depth = 8; /* AGA is less than or equal to 8bit */
    }

    if ((CGXVideoBase != NULL) && (prefs.overlay == PREFS_ON) && (screentype = CGFX)) {
      if ((mode == REFRESH_YUV420) || (prefs.colormode == PREFS_GRAY)) {
        VLH = CreateVLayerHandleTags(window->WScreen,
                                     VOA_SrcType, SRCFMT_YCbCr16,
                                     VOA_SrcWidth, width,
                                     VOA_SrcHeight, height,
                                     VOA_Error, &error,
                                     TAG_DONE);
      } else {
        VLH = CreateVLayerHandleTags(window->WScreen,
                                     VOA_SrcType, SRCFMT_RGB16PC,
                                     VOA_SrcWidth, width,
                                     VOA_SrcHeight, height,
                                     VOA_Error, &error,
                                     TAG_DONE);
      }

      if (VLH != NULL) {
        if (!AttachVLayerTags(VLH, window, TAG_DONE)) {
          use_vlayer = 1;
          if(LockVLayer(VLH)) {
            vlayerimage = (unsigned long *)GetVLayerAttr(VLH, VOA_BaseAddress);
            UnlockVLayer(VLH);
          }
        } else {
          amp_printf("could not attach VLayer\n");
          DeleteVLayerHandle(VLH);
          VLH = NULL;
          return -1;
        }
      } else {
        amp_printf("could not create VLayer handle (%ld)\n", error);
        return -1;
      }
    }

    if ((use_vlayer == 0) && (CyberGfxBase != NULL) && (screentype == CGFX)) {
      window_bitmap = AllocBitMap(width, height, my_depth, BMF_CLEAR|BMF_DISPLAYABLE|BMF_INTERLEAVED|BMF_MINPLANES, window->RPort->BitMap);

      UnLockBitMap(LockBitMapTags(window_bitmap,
                                  LBMI_PIXFMT, (ULONG)&pixfmt,
                                  LBMI_BASEADDRESS, (ULONG)&baseaddress[free_sbuffer],
                                  LBMI_BYTESPERROW, (ULONG)&bpr,
                                  LBMI_BYTESPERPIX, (ULONG)&bpp,
                                  TAG_END));
    }
    prefs.gray_depth = 8; /* Always use as many grays as possible for window playback */
  } else {
    /* Get screenmode */
    dispid = get_screenmode_ID(&screen_width, &screen_height);

    if (dispid == 0) {
      return -1;
    }

    screentype = AGA;

    /* Disable HAM for 8bit movies */
    if ((mode == REFRESH_LUT8) && (prefs.colormode == PREFS_HAM)) {
      prefs.colormode = PREFS_COLOR;
    }

    if (CyberGfxBase != NULL) {
      if (IsCyberModeID(dispid)) {
        screentype = CGFX;
      }
    }

    screen_height &= ~1; /* mod 2 */
    if (screentype == AGA) {
      screen_width &= ~63; /* mod 64 */
    } else {
      screen_width &= ~15; /* mod 16 */
    }

    /* Open requested screen, or no screen at all */
    if (screentype == AGA) {
      if (open_screen_aga(screen_width, screen_height, dispid) < 0) {
        return -1;
      }
    } else if (screentype == CGFX) {
      prefs.half = PREFS_OFF; /* Not supported for CGFX */
      prefs.gray_depth = 8; /* CGFX only support 8bit screens */
      if (prefs.colormode == PREFS_HAM) {
        prefs.colormode = PREFS_COLOR;
      }
      if (open_screen_cgfx(screen_width, screen_height, dispid) < 0) {
        return -1;
      }
    }

    /* Open window */
    window = OpenWindowTags(NULL,
                 WA_CustomScreen,(int)screen,
                 WA_Width,screen->Width,
                 WA_Height,screen->Height,
                 WA_Flags,WFLG_RMBTRAP,
                 WA_IDCMP,IDCMP_RAWKEY,
                 WA_Backdrop,TRUE,
                 WA_Borderless,TRUE,
                 WA_Activate,TRUE,
                 TAG_DONE);

    if (window == NULL) {
      amp_printf("could not open the window\n");
      return -1;
    }

    if ((screentype == CGFX) && (pixfmt != PIXFMT_LUT8)) {
      /* Clear the screen with black pixels */
      FillPixelArray(&screen->RastPort, 0, 0, screen->Width, screen->Height, 0x00000000);
    }

    if ((screentype == AGA) && (prefs.colormode == PREFS_HAM)) {
      unsigned char val;

      if (prefs.triple == PREFS_ON) {
        b = 3;
      } else {
        b = 1;
      }

      for (i=0; i<b; i++) {
        /* Init planes for fast HAM modes */
        if (prefs.ham_width == 4) {
          val = 0xcc;
        } else {
          val = 0x77;
        }
        memset(agabitmap[i].Planes[prefs.ham_depth - 1], val, RASSIZE(screen->Width, screen->Height));

        if (prefs.ham_width == 4) {
          val = 0x77;
        } else {
          val = 0xdd;
        }
        memset(agabitmap[i].Planes[prefs.ham_depth - 2], val, RASSIZE(screen->Width, screen->Height));
      }
    }

    /* Hide the pointer */
    EmptyPointer = AllocVec(512, MEMF_CHIP|MEMF_CLEAR);
    if (EmptyPointer != NULL) {
      SetPointer(window, EmptyPointer, 1, 1, 0, 0);
    }

    use_vlayer = 0;

    if (screentype == CGFX) {
      if (pixfmt == PIXFMT_LUT8) {
        amiga_depth = 8;
      } else {
        amiga_depth = 32;
      }

      if ((CGXVideoBase != NULL) && (prefs.overlay == PREFS_ON)) {
        if ((mode == REFRESH_YUV420) || (prefs.colormode == PREFS_GRAY)) {
          VLH = CreateVLayerHandleTags(screen,
                                       VOA_SrcType, SRCFMT_YCbCr16,
                                       VOA_SrcWidth, width,
                                       VOA_SrcHeight, height,
                                       VOA_Error, &error,
                                       TAG_DONE);
        } else {
          VLH = CreateVLayerHandleTags(screen,
                                       VOA_SrcType, SRCFMT_RGB16PC,
                                       VOA_SrcWidth, width,
                                       VOA_SrcHeight, height,
                                       VOA_Error, &error,
                                       TAG_DONE);
        }

        if (VLH != NULL) {
          if (!AttachVLayerTags(VLH, window, TAG_DONE)) {
            use_vlayer = 1;
            if(LockVLayer(VLH)) {
              vlayerimage = (unsigned long *)GetVLayerAttr(VLH, VOA_BaseAddress);
              UnlockVLayer(VLH);
            }
          } else {
            amp_printf("could not attach VLayer\n");
            DeleteVLayerHandle(VLH);
            VLH = NULL;
            return -1;
          }
        } else {
          amp_printf("could not create VLayer handle (%ld)\n", error);
          return -1;
        }
      }
    } else {
      if (prefs.colormode == PREFS_HAM) {
        amiga_depth = 32;
      } else {
        amiga_depth = 8;
      }
    }
  }

  /* Get refresh mode */
  refresh_rect.pixel_mode = 0;
  i = 0;
  while (mode_conv[i].refresh_mode != 0) {
    if (pixfmt == mode_conv[i].cgfx_mode) {
      refresh_rect.pixel_mode = mode_conv[i].refresh_mode;
      break;
    }
    i++;
  }

  if ((use_vlayer == 0) && (refresh_rect.pixel_mode == 0)) {
    amp_printf("unsupported pixel format %s\n", pixformats[pixfmt]);
    return -1;
  }

  amp_printf("Video: %dx%d\n", width, height);

  if (screen != NULL) {
    verbose_printf("Screen: %dx%d\n", screen->Width, screen->Height);
  } else {
    verbose_printf("Window: %dx%d\n", width, height);
  }

  if (screentype == CGFX) {
    verbose_printf("Pixel Format: %s\n", pixformats[pixfmt]);
  } else {
    verbose_printf("Pixel Format: PLANAR\n");
  }

  /* init/exit */
  refresh_rect.init = NULL;
  refresh_rect.exit = NULL;
  if (screen != NULL) {
    if (use_vlayer == 1) {
      printf("OVERLAY screen\n");
      refresh_rect.init = overlay_refresh_init;
      refresh_rect.exit = overlay_refresh_exit;
    } else if (prefs.triple == PREFS_ON) {
      printf("TRIPLE screen\n");
      refresh_rect.init = triple_refresh_init;
    } else {
      printf("SINGLE screen\n");
    }
  } else {
    if (use_vlayer == 1) {
      printf("OVERLAY window\n");
      refresh_rect.init = overlay_refresh_init;
      refresh_rect.exit = overlay_refresh_exit;
    } else if (screentype == CGFX) {
      printf("CGFX window\n");
      refresh_rect.exit = window_refresh_exit;
    } else {
      printf("AGA window\n");
      aga_window_buffer = malloc(refresh_rect.width * refresh_rect.height);
      refresh_rect.exit = aga_window_refresh_exit;
    }
  }

  /* size */
  if (screen != NULL) {
    refresh_rect.screen_width = screen->Width;
    refresh_rect.screen_height = screen->Height;
  } else {
    refresh_rect.screen_width = refresh_rect.width;
    refresh_rect.screen_height = refresh_rect.height;
  }

  refresh_rect.dst = (unsigned long *)baseaddress[free_sbuffer];

  if (use_vlayer == 1) {
    refresh_rect.dst = vlayerimage;
    bpr = refresh_rect.width * 2;
    refresh_rect.init();
    for (i=0; i<refresh_rect.screen_width * refresh_rect.screen_height / 2; i++) {
      refresh_rect.dst[i] = 0x00800080;
    }
    refresh_rect.exit();
  }

  if (screentype == AGA) {
    bpp = 1;
    if (screen != NULL) {
      refresh_rect.dst = (unsigned long *)agabitmap[free_sbuffer].Planes[0];
      bpr = screen->Width;
    } else {
      refresh_rect.dst = aga_window_buffer;
      bpr = refresh_rect.width;
    }
  }

  refresh_rect.bpr = bpr;
  refresh_rect.bpp = bpp;

  if (use_vlayer == 1) {
    refresh_rect.screen_type = TYPE_OVERLAY;
  } else if (screentype == AGA) {
    refresh_rect.screen_type = TYPE_PLANAR;
  } else {
    refresh_rect.screen_type = TYPE_CHUNKY;
  }

  if (amiga_depth == 8) {
    return 8;
  } else {
    return 32;
  }
}

void open_audio_window(char *text)
{
  /* Window already open? */
  if (window != NULL) {
    return;
  }

  init_gadgets();

  window_text = strdup(text);

  BotGad.Width = -6;
  window = OpenWindowTags(NULL,
                          WA_Title, prefs.window_title,
                          WA_ScreenTitle, prefs.registered,
                          WA_Flags, AMP_FLAGS,
                          WA_IDCMP, AMP_IDCMP|IDCMP_CHANGEWINDOW,
                          WA_Gadgets, &BotGad,
                          WA_Left, 100,
                          WA_Top, 100,
                          WA_Width, 220,
                          WA_Height, 42,
                          WA_Activate, TRUE,
                          TAG_DONE);

  if (window == NULL) {
    amp_printf("could NOT open window\n");
    return;
  } else {
    struct IntuiMessage *imsg = NULL;
    ULONG imCode, imClass;

    /* Resize window and set pens */
    int length = TextLength(window->RPort, window_text, strlen(window_text));
    ChangeWindowBox(window, 100, 100, window->BorderLeft+length+window->BorderRight+8, window->BorderTop+window->IFont->tf_YSize*2+window->BorderBottom);
    SetAPen(window->RPort, 1);
    SetBPen(window->RPort, 0);

    /* Wait until window has been resized */
    for(;;) {
      /* Wait for IDCMP message */
      WaitPort(window->UserPort);

      /* Check for IDCMP messages */
      if ((imsg = (struct IntuiMessage *)GetMsg(window->UserPort))) {
        imClass = imsg->Class;
        imCode = imsg->Code;

        ReplyMsg((struct Message *)imsg);

        if (imClass == IDCMP_CHANGEWINDOW) {
          Move(window->RPort, 4, window->IFont->tf_YSize);
          Text(window->RPort, window_text, strlen(window_text));
          break; /* We are done waiting */
        }
      }
    }
  }
}

static char chapter[3] = "00\0"; /* FIXME */

int dvd_input_hack = -1;

int check_window(int new_position, int do_wait)
{
  struct IntuiMessage *imsg = NULL;
  ULONG imCode, imClass;
  int ret = CHECK_OK;

  /* No window !? Then quit! */
  if (window == NULL) {
    return CHECK_QUIT;
  }

  if (do_wait) {
    WaitPort(window->UserPort);
  }

  dvd_input_hack = -1;

  /* Check for IDCMP messages */
  if ((imsg = (struct IntuiMessage *)GetMsg(window->UserPort))) {
    imClass = imsg->Class;
    imCode = imsg->Code;

    ReplyMsg((struct Message *)imsg);

    if (imClass == IDCMP_CLOSEWINDOW) {
      ret = CHECK_QUIT;
    } else if (imClass == IDCMP_RAWKEY) {
      if (imCode & 0x80) {
        /* key is released */
        imCode &= 0x7f;

        dvd_input_hack = imCode;

        switch (imCode) {
          /* ESC */
          case 69:
            ret = CHECK_QUIT;
            break;

          /* 1 - 9 */
          case 1: case 2: case 3:
          case 4: case 5: case 6:
          case 7: case 8: case 9:
            ret = imCode * 10;
            break;

          /*
            #define KEY_0_PAD 15
            #define KEY_1_PAD 29
            #define KEY_2_PAD 30
            #define KEY_3_PAD 31
            #define KEY_4_PAD 45
            #define KEY_5_PAD 46
            #define KEY_6_PAD 47
            #define KEY_7_PAD 61
            #define KEY_8_PAD 62
            #define KEY_9_PAD 63
            #define KEY_DEL_PAD 60
            #define KEY_ENTER_PAD 67
            #define KEY_BACKSPACE 65
          */

          case 15:
            chapter[0] = chapter[1];
            chapter[1] = '0';
            break;

          case 29: case 30: case 31:
            chapter[0] = chapter[1];
            chapter[1] = '0' + (imCode - 29 + 1);
            break;

          case 45: case 46: case 47:
            chapter[0] = chapter[1];
            chapter[1] = '0' + (imCode - 45 + 4);
            break;

          case 61: case 62: case 63:
            chapter[0] = chapter[1];
            chapter[1] = '0' + (imCode - 61 + 7);
            break;

          case 60: case 65:
            chapter[0] = '0';
            chapter[1] = '0';
            break;

          case 67:
            sscanf(chapter, "%d", &ret);
            debug_printf("CHAPTER: %d\n", ret);
            chapter[0] = '0';
            chapter[1] = '0';
            break;

          /* 0 */
          case 10:
            ret = 0;
            break;

          case 64:
            ret = CHECK_PAUSE;
            break;

          case 68:
            ret = CHECK_CONT;
            if (amiga_is_down) {
              ret = CHECK_FLIP;
            }
            break;

          case 102: case 103:
            amiga_is_down = 0;
            break;

/* DVD hack
#define KEY_LEFT 79
#define KEY_RIGHT 78
#define KEY_UP 76
#define KEY_DOWN 77
#define KEY_TAB 66

          case KEY_LEFT:
          case KEY_RIGHT:
          case KEY_UP:
          case KEY_DOWN:
          case KEY_TAB:
            ret = imCode;
            break;
*/

          default:
            break;
        }
      } else {
        switch (imCode) {
          case 102: case 103:
            amiga_is_down = 1;
            break;

          default:
            break;
        }
      }
    } else if (imClass == IDCMP_GADGETUP) {
      down = 0;
      ret = ((BotGadInfo.HorizPot) * 100) / 65535; /* I want % */
    }
    else if (imClass == IDCMP_GADGETDOWN) {
      down = 1;
    }
  }

  if ((down == 0) && (new_position >= 0)) {
    /* This test belongs here */
    if (new_position > 65535) {
      new_position = 65535;
    }
    NewModifyProp(&BotGad, window, NULL, AUTOKNOB | FREEHORIZ | PROPNEWLOOK, new_position, 0, MAXBODY/64, 0, 1);
  }

  return ret;
}

void set_palette(int colors)
{
  int i;

  if (amiga_depth == 8) {
    if (prefs.window == PREFS_ON) {
      static int pens_allocated = 0;
      int R, G, B;

      if (pens_allocated == 1) {
        for (i=0; i<256; i++) {
          ReleasePen(window->WScreen->ViewPort.ColorMap, refresh_rect.pixel[i]);
        }
      }

      memset(refresh_rect.pixel, 0, 256);

      for(i=0; i<256; i++) {
        R = (refresh_rect.palette[i]<< 8)&0xff000000;
        G = (refresh_rect.palette[i]<<16)&0xff000000;
        B = (refresh_rect.palette[i]<<24)&0xff000000;
        refresh_rect.pixel[i] = ObtainBestPen(window->WScreen->ViewPort.ColorMap, R, G, B, TAG_DONE);
      }

      pens_allocated = 1;
    } else {
      for(i=0; i<colors; i++) {
        rgbtab[1+i*3+0] = (refresh_rect.palette[i]<< 8)&0xff000000;
        rgbtab[1+i*3+1] = (refresh_rect.palette[i]<<16)&0xff000000;
        rgbtab[1+i*3+2] = (refresh_rect.palette[i]<<24)&0xff000000;
      }

      rgbtab[0] = (colors<<16)|0;
      rgbtab[1+colors*3] = 0;
      LoadRGB32(&screen->ViewPort, rgbtab);
    }
  }
}
