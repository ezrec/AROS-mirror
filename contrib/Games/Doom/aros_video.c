#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <intuition/intuition.h>
#include <libraries/asl.h>
#include <cybergraphx/cybergraphics.h>
#include <devices/gameport.h>
#include <devices/keymap.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/keymap.h>
#include <proto/cybergraphics.h>
#include <proto/timer.h>
#include <clib/alib_protos.h>

#include <aros/debug.h>

#include "doomtype.h"
#include "doomdef.h"
#include "doomstat.h"
#include "i_system.h"
#include "i_video.h"
#include "v_video.h"
#include "m_argv.h"
#include "m_bbox.h"
#include "d_main.h"

#include "amiga_sega.h"
#include "r_draw.h"
#include "w_wad.h"
#include "z_zone.h"

/**********************************************************************/

extern struct ExecBase *SysBase;
extern struct Device *TimerBase;
struct Library *AslBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *LowLevelBase = NULL;
struct Library *KeymapBase = NULL;

extern int cpu_type;

int SCREENWIDTH;
int SCREENHEIGHT;
int weirdaspect;


#define NUMPALETTES 14

static struct ScreenModeRequester *video_smr = NULL;
static struct Screen *video_screen = NULL;
static struct Window *video_window = NULL;

static struct ScreenBuffer *SBuffer[3] = {NULL, NULL, NULL};
static struct ScreenBuffer *NextSBuffer = NULL;

static int video_mod = 320;
static ULONG video_pixfmt = PIXFMT_LUT8;
static ULONG *video_buffer = NULL;

static int video_depth = 8;

static FAR ULONG video_colourtable[NUMPALETTES][1 + 3*256 + 1];
static FAR ULONG cgfx_coltab[NUMPALETTES][256];
static int video_palette_index = 0;
static int video_pending_palette_index = 0;
static volatile WORD video_palette_changed = 0;

static BOOL video_is_cyber_mode = FALSE;
static BOOL video_is_directcgx = FALSE;
static BOOL video_is_native = FALSE;
static BOOL video_do_screen = FALSE;
static BOOL video_doing_fps = FALSE;
static APTR video_bitmap_handle;

static struct RastPort video_temprp;
static struct BitMap video_tmp_bm = {
  0, 0, 0, 0, 0, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

static UWORD *emptypointer;

/* keyboard */
static BOOL video_is_rawkey;

/* gameport stuff */
static struct MsgPort *gameport_mp = NULL;
static struct IOStdReq *gameport_io = NULL;
static BOOL gameport_is_open = FALSE;
static BOOL gameport_io_in_progress = FALSE;
static struct InputEvent gameport_ie;
static BYTE gameport_ct;		/* controller type */
struct GamePortTrigger gameport_gpt = {
  GPTF_UPKEYS | GPTF_DOWNKEYS,	/* gpt_Keys */
  0,				/* gpt_Timeout */
  1,				/* gpt_XDelta */
  1				/* gpt_YDelta */
};

/* SEGA variables */
static ULONG prevSega;
static BOOL sega3_selected = FALSE;
static BOOL sega6_selected = FALSE;

#ifdef PROFILE
unsigned int profile[32][4];
#endif

/****************************************************************************/

static unsigned int start_time[2];
static unsigned int wpa8_time = 0;
static unsigned int lock_time = 0;
static unsigned int c2p_time = 0;
static unsigned int total_frames = 0;

/****************************************************************************/

static __inline void start_timer (void)
{
     struct timeval tv;
     
     GetSysTime(&tv);
     
     start_time[0] = tv.tv_secs;
     start_time[1] = tv.tv_micro;
     
}

/****************************************************************************/

static __inline unsigned int end_timer (void)
{
  unsigned int end_time[2], result;
  struct timeval tv;
  
  GetSysTime(&tv);
  
  end_time[0] = tv.tv_secs;
  end_time[1] = tv.tv_micro;
  
  result = (end_time[0] - start_time[0]) * 1000000 +
  	   ( ((int)end_time[1]) -  ((int)start_time[1]) );

  return result;
}


/**********************************************************************/
static void video_do_fps (struct RastPort *rp, int yoffset)
{
  #define FPS_STAT_COUNT 50
  
  static ULONG oldsecs = 0;
  static LONG oldmicro = 0;
  static LONG fps_stat[FPS_STAT_COUNT];
  static LONG fps_stat_pos;
  struct timeval tv;
  ULONG secs;
  LONG micro;
  LONG time, fps = 0, i;
  char msg[4];
  
  GetSysTime(&tv);
  secs  = tv.tv_secs;
  micro = tv.tv_micro;
  
  time = (secs - oldsecs) * 1000000 + (micro - oldmicro);
  if (time > 0) fps = 1000000 / time;
  
  fps_stat[fps_stat_pos++] = fps;
  fps_stat_pos %= FPS_STAT_COUNT;
  
  fps = 0;
  for (i = 0; i < FPS_STAT_COUNT; i++)
  {
    fps += fps_stat[i];
  }
  fps /= FPS_STAT_COUNT;
  
  msg[0] = (fps % 1000) / 100 + '0';
  msg[1] = (fps % 100) / 10 + + '0';
  msg[2] = (fps % 10) + '0';
  msg[3] = '\0';

  SetABPenDrMd(rp, 2, 1, JAM2);
  
  Move (rp, video_window->BorderLeft + SCREENWIDTH - 24,
  	    video_window->BorderTop  + yoffset + rp->TxBaseline + 3);
  Text (rp, msg, 3);
    
  oldsecs  = secs;
  oldmicro = micro;
}

/**********************************************************************/
// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics (void)
{
  ULONG propertymask, idcmp, wflags, width, pixfmt;
  struct Rectangle rect;
  char reqtitle[32];
  int mode, depth, nbytes, p;
  DisplayInfoHandle handle;
  struct DisplayInfo dispinfo;
  struct DimensionInfo dimsinfo;
  static struct TextAttr topaz8 = {
    "topaz.font", 8, FS_NORMAL, FPF_ROMFONT
  };

  if ((KeymapBase = OpenLibrary ("keymap.library", 0)) == NULL)
    I_Error ("Can't open keymap.library");

  video_doing_fps = M_CheckParm ("-fps");
  video_do_screen = M_CheckParm ("-scr");
  
#ifndef AROS
kprintf("_-- Asl Requester\n");
    if (AslBase == NULL) {
      if ((AslBase = OpenLibrary ("asl.library", 37L)) == NULL ||
          (video_smr = AllocAslRequestTags (ASL_ScreenModeRequest, TAG_DONE)) == NULL) {
        I_Error ("OpenLibrary(""asl.library"", 37) failed");
      }
    }
kprintf("--- Asl Requester done\n");
#endif

    CyberGfxBase = OpenLibrary ("cybergraphics.library", 0); /* may be NULL */
    if (!CyberGfxBase) video_do_screen = TRUE;
    
    if (video_do_screen)
    {
	/* first check tooltypes for SCREENMODE */
	mode = -1;

	/* if not found in icon tooltypes, then put up a ScreenMode requester */
	if (mode == -1) {
	  propertymask = DIPF_IS_EXTRAHALFBRITE | DIPF_IS_DUALPF |
                	 DIPF_IS_PF2PRI | DIPF_IS_HAM;
    #ifndef AROS 
	  if (CyberGfxBase != NULL)
            mode = BestCModeIDTags (CYBRBIDTG_NominalWidth, SCREENWIDTH,
                                    CYBRBIDTG_NominalHeight, SCREENHEIGHT,
                                    CYBRBIDTG_Depth,    8,
                                    TAG_DONE);
	  else if (GfxBase->LibNode.lib_Version >= 39)
            mode = BestModeID (BIDTAG_NominalWidth,     SCREENWIDTH,
                               BIDTAG_NominalHeight,    SCREENHEIGHT,
                               BIDTAG_Depth,            video_depth,
                               BIDTAG_DIPFMustNotHave,  propertymask,
                               TAG_DONE);
	  if (AslBase->lib_Version >= 38) {
            sprintf (reqtitle, "ADoomPPC %dx%d", SCREENWIDTH, SCREENHEIGHT);
            if (!AslRequestTags (video_smr,
                        	 ASLSM_TitleText,        (ULONG)reqtitle,
                        	 ASLSM_InitialDisplayID, mode,
                        	 ASLSM_MinWidth,         SCREENWIDTH,
                        	 ASLSM_MinHeight,        SCREENHEIGHT,
                        	 ASLSM_MaxWidth,         MAXSCREENWIDTH,
                        	 ASLSM_MaxHeight,        MAXSCREENHEIGHT,
                        	 ASLSM_MinDepth,         8,
                        	 ASLSM_MaxDepth,         8,
                        	 ASLSM_PropertyMask,     propertymask,
                        	 ASLSM_PropertyFlags,    0,
                        	 TAG_DONE)) {
              I_Error ("AslRequest() failed");
            }
            mode = video_smr->sm_DisplayID;
	  }
    #endif      
	}

    #ifdef AROS
	video_is_native = FALSE;
    #else
	if ((handle = FindDisplayInfo (mode)) == NULL) {
	  I_Error ("Can't FindDisplayInfo() for mode %08x", mode);
	}
	if ((nbytes = GetDisplayInfoData (handle, (UBYTE *)&dispinfo,
                                	  sizeof(struct DisplayInfo), DTAG_DISP,
                                	  NULL)) < 40 /*sizeof(struct DisplayInfo)*/) {
	  I_Error ("Can't GetDisplayInfoData() for mode %08x, got %d bytes",
        	   mode, nbytes);
	}
	if ((nbytes = GetDisplayInfoData (handle, (UBYTE *)&dimsinfo,
                                	  sizeof(dimsinfo), DTAG_DIMS,
                                	  NULL)) < 66 /* sizeof(dimsinfo)*/) {
	  I_Error ("Can't GetDisplayInfoData() for mode %08x, got %d bytes",
        	   mode, nbytes);
	}

	video_is_cyber_mode = 0;
	if (CyberGfxBase != NULL)
	  video_is_cyber_mode = IsCyberModeID (mode);

	video_is_native = ((GfxBase->LibNode.lib_Version < 39 ||
                	  (dispinfo.PropertyFlags & DIPF_IS_EXTRAHALFBRITE) != 0 ||
                	  (dispinfo.PropertyFlags & DIPF_IS_AA) != 0 ||
                	  (dispinfo.PropertyFlags & DIPF_IS_ECS) != 0 ||
                	  (dispinfo.PropertyFlags & DIPF_IS_DBUFFER) != 0) &&
                	  !video_is_cyber_mode &&
                	  (dispinfo.PropertyFlags & DIPF_IS_FOREIGN) == 0);

	/* manual overrides */
	if (M_CheckParm ("-rtg"))
	  video_is_native = FALSE;
	else if (M_CheckParm ("-native"))
	  video_is_native = TRUE;
    #endif

	printf ("Screen Mode $%08x is ", mode);
	printf (" 8-BIT");
	if (video_is_native)
	  printf (" Native-Planar");
	if (video_is_cyber_mode)
	  printf (" CyberGraphX");
	printf ("\n");

	video_depth = 8;

	rect.MinX = 0;
	rect.MinY = 0;
	rect.MaxX = SCREENWIDTH - 1;
	rect.MaxY = SCREENHEIGHT - 1;

    #ifndef AROS
	if (video_is_cyber_mode && M_CheckParm ("-directcgx"))
	  video_is_directcgx = TRUE;
    #endif

	if ((video_screen = OpenScreenTags (NULL,
              SA_Type,        CUSTOMSCREEN,
              SA_DisplayID,   mode,
              SA_DClip,       (ULONG)&rect,
              SA_Width,       SCREENWIDTH,
              SA_Height,      SCREENHEIGHT,
              SA_Depth,       video_depth,
              SA_Font,        &topaz8,
              /* SA_Draggable,FALSE, */
              /* SA_AutoScroll,FALSE, */
              /* SA_Exclusive,TRUE, */
              SA_Quiet,       TRUE,
              TAG_DONE,       0)) == NULL) {
	  I_Error ("OpenScreen() failed");
	}

	if (video_is_directcgx) {
	  video_bitmap_handle = LockBitMapTags (video_screen->ViewPort.RasInfo->BitMap,
                                        	LBMI_WIDTH,       &width,
                                        	LBMI_DEPTH,       &depth,
                                        	LBMI_PIXFMT,      &pixfmt,
                                        	TAG_DONE);
	  UnLockBitMap (video_bitmap_handle);
	  if (/* width != SCREENWIDTH || */
              depth != video_depth ||
              pixfmt != PIXFMT_LUT8) {
            I_Error ("ScreenMode of width %d, depth %d, pixfmt %d cannot be used with -directcgx",
                     width, depth, pixfmt);
	  }
	}
	
  } /* video_do_screen */
  
  idcmp = IDCMP_RAWKEY;
  wflags = WFLG_ACTIVATE | WFLG_BORDERLESS | WFLG_RMBTRAP | WFLG_NOCAREREFRESH |
           WFLG_SIMPLE_REFRESH;
  if (M_CheckParm("-mouse") != NULL) {
    idcmp |= IDCMP_MOUSEMOVE | IDCMP_DELTAMOVE | IDCMP_MOUSEBUTTONS;
    wflags |= WFLG_REPORTMOUSE;
  }

  if (video_do_screen)
  {
    if ((video_window = OpenWindowTags (NULL,
          WA_Left,         0,
          WA_Top,          0,
          WA_Width,        SCREENWIDTH,
          WA_Height,       SCREENHEIGHT,
          WA_IDCMP,        idcmp,
          WA_Flags,        wflags,
          WA_CustomScreen, (IPTR)video_screen,
          TAG_DONE)) == NULL) {
      I_Error ("OpenWindow() failed");
    }
  } else {
    struct Screen *scr = LockPubScreen(NULL);
    STRPTR wintitle = "AROS Doom";
    WORD winx = 0, winy = 0;
    int p;
    
    if (scr == NULL) I_Error("Can't lock public screen!");
    
    if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) <= 8)
    {
       UnlockPubScreen(NULL, scr);
       I_Error("Window mode only works on hi or true color screens. Try option \"-scr\" to play Doom on a screen!");
    }
    
    wflags &= ~WFLG_BORDERLESS;
    idcmp |= IDCMP_CLOSEWINDOW;
    
    p = M_CheckParm("-winleft");
    if (p && p < myargc-1) winx = atoi(myargv[p+1]);
    
    p = M_CheckParm("-wintop");
    if (p && p < myargc-1) winy = atoi(myargv[p+1]);
    
    p = M_CheckParm("-wintitle");
    if (p && p < myargc-1) wintitle = myargv[p+1];
    
    video_window = OpenWindowTags(NULL,
    	  WA_PubScreen,		(IPTR)scr,
    	  WA_Title,		(IPTR)wintitle,
          WA_Left,        	winx,
          WA_Top,        	winy,
          WA_InnerWidth,        SCREENWIDTH,
          WA_InnerHeight,       SCREENHEIGHT,
	  WA_AutoAdjust,    	TRUE,
          WA_IDCMP,       	idcmp,
          WA_Flags,       	wflags,
	  WA_CloseGadget,	TRUE,
	  WA_DepthGadget,	TRUE,
	  WA_DragBar,		TRUE,
          TAG_DONE);

    UnlockPubScreen(NULL, scr);
    	  
    if (video_window == NULL) {
      I_Error ("OpenWindow() failed");
    }

  }
    
  InitBitMap (&video_tmp_bm, video_depth, SCREENWIDTH, 1);
  for (depth = 0; depth < video_depth; depth++)
    if ((video_tmp_bm.Planes[depth] = (PLANEPTR)AllocRaster (SCREENWIDTH, 1)) == NULL)
      I_Error ("AllocRaster() failed");
  video_temprp = *video_window->RPort;
  video_temprp.Layer = NULL;
  video_temprp.BitMap = &video_tmp_bm;

  if ((emptypointer = AllocVec (16, MEMF_CHIP+MEMF_CLEAR)) == NULL)
    I_Error ("Couldn't allocate chip memory for pointer");

  if (!M_CheckParm ("-mousepointer"))
    SetPointer (video_window, emptypointer, 1, 16, 0, 0);

  I_RecalcPalettes ();

  /* keyboard & joystick initialisation */

  video_is_rawkey = M_CheckParm ("-rawkey");

  if (M_CheckParm ("-sega3") != NULL)
    sega3_selected = TRUE;

  if (M_CheckParm ("-sega6") != NULL)
    sega6_selected = TRUE;

  if (M_CheckParm ("-joypad") != NULL) {

    if ((LowLevelBase = OpenLibrary ("lowlevel.library", 0)) == NULL)
      I_Error ("-joypad option specified and can't open lowlevel.library");

  } else {

#ifndef AROS
    if ((gameport_mp = CreatePort (NULL, 0)) == NULL ||
        (gameport_io = (struct IOStdReq *)CreateExtIO (gameport_mp,
                                              sizeof(struct IOStdReq))) == NULL ||
        OpenDevice ("gameport.device", 1, (struct IORequest *)gameport_io, 0) != 0)
      I_Error ("Can't open gameport.device");

    gameport_is_open = TRUE;

    Forbid ();

    gameport_io->io_Command = GPD_ASKCTYPE;
    gameport_io->io_Length = 1;
    gameport_io->io_Data = &gameport_ct;
    DoIO ((struct IORequest *)gameport_io);

    if (gameport_ct != GPCT_NOCONTROLLER) {

      Permit ();
      fprintf (stderr, "Another task is using the gameport!  Joystick disabled");
      CloseDevice ((struct IORequest *)gameport_io);
      gameport_is_open = FALSE;

    } else {

      gameport_ct = GPCT_ABSJOYSTICK;
      gameport_io->io_Command = GPD_SETCTYPE;
      gameport_io->io_Length = 1;
      gameport_io->io_Data = &gameport_ct;
      DoIO ((struct IORequest *)gameport_io);

      Permit ();

      gameport_io->io_Command = GPD_SETTRIGGER;
      gameport_io->io_Length = sizeof(struct GamePortTrigger);
      gameport_io->io_Data = &gameport_gpt;
      DoIO ((struct IORequest *)gameport_io);

      gameport_io->io_Command = GPD_READEVENT;
      gameport_io->io_Length = sizeof (struct InputEvent);
      gameport_io->io_Data = &gameport_ie;
      SendIO ((struct IORequest *)gameport_io);
      gameport_io_in_progress = TRUE;
    }
#endif

  }
  
}

/**********************************************************************/
void I_ShutdownGraphics (void)
{
  int depth;

  if (LowLevelBase != NULL) {
    CloseLibrary (LowLevelBase);
    LowLevelBase = NULL;
  }
  if (gameport_io_in_progress) {
    AbortIO ((struct IORequest *)gameport_io);
    WaitIO ((struct IORequest *)gameport_io);
    gameport_io_in_progress = FALSE;
    gameport_ct = GPCT_NOCONTROLLER;
    gameport_io->io_Command = GPD_SETCTYPE;
    gameport_io->io_Length = 1;
    gameport_io->io_Data = &gameport_ct;
    DoIO ((struct IORequest *)gameport_io);
  }
  if (gameport_is_open) {
    CloseDevice ((struct IORequest *)gameport_io);
    gameport_is_open = FALSE;
  }
  if (gameport_io != NULL) {
    DeleteExtIO ((struct IORequest *)gameport_io);
    gameport_io = NULL;
  }
  if (gameport_mp != NULL) {
    DeletePort (gameport_mp);
    gameport_mp = NULL;
  }

  if (video_window != NULL) {
    ClearPointer (video_window);
    CloseWindow (video_window);
    video_window = NULL;
  }
  if (emptypointer != NULL) {
    FreeVec (emptypointer);
    emptypointer = NULL;
  }

  if (video_screen != NULL) {
    CloseScreen (video_screen);
    video_screen = NULL;
  }

  for (depth = 0; depth < video_depth; depth++) {
    if (video_tmp_bm.Planes[depth] != NULL) {

      FreeRaster (video_tmp_bm.Planes[depth], SCREENWIDTH, 1);

      video_tmp_bm.Planes[depth] = NULL;
    }
  }

  if (KeymapBase == NULL) {
    CloseLibrary (KeymapBase);
    KeymapBase = NULL;
  }

  if (CyberGfxBase == NULL) {
    CloseLibrary (CyberGfxBase);
    CyberGfxBase = NULL;
  }

}

/**********************************************************************/
// recalculate colourtable[][] after changing usegamma
void I_RecalcPalettes (void)
{
  int p, i;
  byte *palette;
  static int lu_palette;

  lu_palette = W_GetNumForName ("PLAYPAL");
  for (p = 0; p < NUMPALETTES; p++) {
      if (video_do_screen)
      {
         palette = (byte *) W_CacheLumpNum (lu_palette, PU_STATIC)+p*768;

	i = 3 * 256 - 1;
	video_colourtable[p][i + 2] = 0;
	do {
          // Better to define c locally here instead of for the whole function:
          ULONG c = gammatable[usegamma][palette[i]];
          c += (c << 8);
          video_colourtable[p][i + 1] = (c << 16) + c;
	} while (--i >= 0);
	video_colourtable[p][0] = (256 << 16) + 0;
      } else {
        palette = (byte *) W_CacheLumpNum (lu_palette, PU_STATIC);
	
        for(i = 0; i < 256;i++)
	{
	    cgfx_coltab[p][i] = ((gammatable[usegamma][palette[i * 3]]) << 16) +
	    			((gammatable[usegamma][palette[i * 3 + 1]]) << 8) +
				((gammatable[usegamma][palette[i * 3 + 2]]));
	}
      }
 }
}

/**********************************************************************/
// Takes full 8 bit values.
void I_SetPalette (byte *palette, int palette_index)
{
  video_palette_changed = 1;
  video_palette_index = palette_index;
}

/**********************************************************************/
// Called by anything that renders to screens[0] (except 3D view)
void I_MarkRect (int left, int top, int width, int height)
{
  M_AddToBox (dirtybox, left, top);
  M_AddToBox (dirtybox, left + width - 1, top + height - 1);
}

/**********************************************************************/
void I_StartUpdate (void)
{
}

/**********************************************************************/
void I_UpdateNoBlit (void)
{
}

/**********************************************************************/
void I_FinishUpdate (void)
/* This needs optimising to copy just the parts that changed,
   especially if the user has shrunk the playscreen. */
{
  int top, left, width, height;
  int stat, i, j;
  UBYTE *base_address;

  total_frames++;

#ifndef AROS  
  if (video_is_directcgx) {
    if (video_palette_changed != 0) {
      LoadRGB32 (&video_screen->ViewPort,
                 video_colourtable[video_palette_index]);
      video_palette_changed = 0;
    }
    start_timer ();
    video_bitmap_handle = LockBitMapTags (video_screen->ViewPort.RasInfo->BitMap,
                                            LBMI_BASEADDRESS, &base_address,
                                            TAG_DONE);
    memcpy (base_address, screens[0], SCREENWIDTH * SCREENHEIGHT);
    UnLockBitMap (video_bitmap_handle);
    lock_time += end_timer();

    if (video_doing_fps)
      video_do_fps (video_window->RPort, 0);
    return;
  }
#endif

  /* update only the viewwindow and dirtybox when gamestate == GS_LEVEL */
  if (gamestate == GS_LEVEL) {
    if (dirtybox[BOXLEFT] < viewwindowx)
      left = dirtybox[BOXLEFT];
    else
      left = viewwindowx;
    if (dirtybox[BOXRIGHT] + 1 > viewwindowx + scaledviewwidth)
      width = dirtybox[BOXRIGHT] + 1 - left;
    else
      width = viewwindowx + scaledviewwidth - left;
    if (dirtybox[BOXBOTTOM] < viewwindowy) /* BOXBOTTOM is really the top! */
      top = dirtybox[BOXBOTTOM];
    else
      top = viewwindowy;
    if (dirtybox[BOXTOP] + 1 > viewwindowy + viewheight)
      height = dirtybox[BOXTOP] + 1 - top;
    else
      height = viewwindowy + viewheight - top;
    M_ClearBox (dirtybox);
#ifdef RANGECHECK
    if (left < 0 || left + width > SCREENWIDTH ||
        top < 0 || top + height > SCREENHEIGHT)
      I_Error ("I_FinishUpdate: Box out of range: %d %d %d %d",
               left, top, width, height);
#endif
  } else {
    left = 0;
    top = 0;
    width = SCREENWIDTH;
    height = SCREENHEIGHT;
  }

  /* not directcgx or graffiti */
  if (video_palette_changed != 0) {
  
    if (video_do_screen)
    {
	LoadRGB32 (&video_screen->ViewPort,
        	   video_colourtable[video_palette_index]);
    }

    video_palette_changed = 0;

  }

  if (video_do_screen)
  {
    start_timer();
    WritePixelArray8 (video_window->RPort, 0, top, SCREENWIDTH-1,
                      top+height-1, &screens[0][SCREENWIDTH * top],
                      &video_temprp);
    wpa8_time += end_timer();
  } else {
    WriteLUTPixelArray(screens[0],
		       left,
		       top,
		       SCREENWIDTH,
		       video_window->RPort,
		       cgfx_coltab[video_palette_index],
		       video_window->BorderLeft,
		       video_window->BorderTop,
		       width,
		       height,
		       CTABFMT_XRGB8);
  }
  
  if (video_doing_fps)
    video_do_fps (video_window->RPort, 0);
    
  SetTaskPri(FindTask(NULL), -1);
  SetTaskPri(FindTask(NULL), 0);

}

/**********************************************************************/
// Wait for vertical retrace or pause a bit.  Use when quit game.
void I_WaitVBL(int count)
{
    Delay(count);
}

/**********************************************************************/
void I_ReadScreen (byte* scr)
{
  memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);

}

/**********************************************************************/
void I_BeginRead (void)
{
}

/**********************************************************************/
void I_EndRead (void)
{
}

/**********************************************************************/

int xlate_key (UWORD rawkey, UWORD qualifier, APTR *eventptr)
{
  char buffer[4], c;
  struct InputEvent ie;
  static int xlate[0x68] = {
    '`', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', KEY_MINUS, KEY_EQUALS, '\\', 0, '0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', KEY_F11, KEY_F12, 0, '0', '2', '3',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',
    'l', ';', '\'', KEY_ENTER, 0, '4', '5', '6',
    KEY_RSHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/', 0, '.', '7', '8', '9',
    ' ', KEY_BACKSPACE, KEY_TAB, KEY_ENTER, KEY_ENTER, KEY_ESCAPE, KEY_F11,
    0, 0, 0, KEY_MINUS, 0, KEY_UPARROW, KEY_DOWNARROW, KEY_RIGHTARROW, KEY_LEFTARROW,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
    KEY_F9, KEY_F10, '(', ')', '/', '*', KEY_EQUALS, KEY_PAUSE,
    KEY_RSHIFT, KEY_RSHIFT, 0, KEY_RCTRL, KEY_LALT, KEY_RALT, 0, KEY_RCTRL
  };

  if (video_is_rawkey)
    if (rawkey < 0x68)
      return xlate[rawkey];
    else
      return 0;
  else
    if (rawkey > 0x00 && rawkey < 0x0a) // '1'..'9', no SHIFT French keyboards
      return '0' + rawkey;
    else if (rawkey == 0x0a)            // '0'
      return '0';
    else if (rawkey < 0x40) {
      ie.ie_Class = IECLASS_RAWKEY;
      ie.ie_Code = rawkey;
      ie.ie_Qualifier = qualifier;
      ie.ie_EventAddress = eventptr ? *eventptr : NULL; /* AROS FIXME: key intuimessage */
      ie.ie_NextEvent = NULL; /* AROS FIX */
      		
      if (MapRawKey (&ie, buffer, sizeof(buffer), NULL) > 0) {
        c = buffer[0];
        if (c >= '0' && c <= '9')       /* numeric pad */
          switch (c) {
          case '0':
            return ' ';
          case '1':
            return ',';
          case '2':
            return KEY_RCTRL;
          case '3':
            return '.';
          case '4':
            return KEY_LEFTARROW;
          case '5':
            return KEY_DOWNARROW;
          case '6':
            return KEY_RIGHTARROW;
          case '7':
            return ',';
          case '8':
            return KEY_UPARROW;
          case '9':
            return '.';
          }
        else if (c >= 'A' && c <= 'Z')
          return c - 'A' + 'a';
        else if (c == '<')
          return ',';
        else if (c == '>')
          return '.';
        else if (c == '-')
          return KEY_MINUS;
        else if (c == '=')
          return KEY_EQUALS;
        else if (c == '[')
          return KEY_F11;
        else if (c == ']')
          return KEY_F12;
        else if (c == '\r')
          return KEY_ENTER;
        else if (c == '\n')
          return KEY_ENTER;
        else
          return c;
      } else
        return 0;
    } else if (rawkey < 0x68)
      return xlate[rawkey];
    else
      return 0;
}

/**********************************************************************/
void amiga_getevents (void)
{
  event_t event;
  ULONG class;
  UWORD code;
  WORD mousex, mousey;
  struct IntuiMessage *msg;
  static ULONG previous = 0;
  static event_t joyevent = {0}, mouseevent = {0};
  ULONG currSega;
  int doomkey;

  if (video_window != NULL) {
    while ((msg = (struct IntuiMessage *)GetMsg (video_window->UserPort)) != NULL) {
      class = msg->Class;
      code = msg->Code;
      mousex = msg->MouseX;
      mousey = msg->MouseY;
      if (class == IDCMP_RAWKEY) {
        if ((code & 0x80) != 0) {
          code &= ~0x80;
	  event.type = ev_keyup;
        } else {
          event.type = ev_keydown;
        }
        if (code < 0x68)
          doomkey = xlate_key (code, msg->Qualifier, msg->IAddress);
      }
      ReplyMsg ((struct Message *)msg);  /* reply after xlating key */
      if (class == IDCMP_RAWKEY) {
        if (code < 0x68 && doomkey != 0) {
          event.data1 = doomkey;
          D_PostEvent (&event);
          /* printf ("key %02x -> %02x\n", code, doomkey); */
        }
      } else if (class == IDCMP_MOUSEMOVE) {
        mouseevent.type = ev_mouse;
        mouseevent.data2 = (mousex << 3);
        mouseevent.data3 = -(mousey << 5);
        D_PostEvent (&mouseevent);
      } else if (class == IDCMP_MOUSEBUTTONS) {
        mouseevent.type = ev_mouse;
        switch (code) {
          case SELECTDOWN:
            mouseevent.data1 |= 1;
            break;
          case SELECTUP:
            mouseevent.data1 &= ~1;
            break;
          case MENUDOWN:
            mouseevent.data1 |= 2;
            break;
          case MENUUP:
            mouseevent.data1 &= ~2;
            break;
          case MIDDLEDOWN:
            mouseevent.data1 |= 4;
            break;
          case MIDDLEUP:
            mouseevent.data1 &= ~4;
            break;
          default:
            break;
        }
        D_PostEvent (&mouseevent);
      } else if (class == IDCMP_CLOSEWINDOW)
      {
        I_Quit();
      }
    }
  }

#ifndef AROS

  if (gameport_is_open && gameport_io_in_progress) {
    while (GetMsg (gameport_mp) != NULL) {
      switch (gameport_ie.ie_Code) {
        case IECODE_LBUTTON:
          joyevent.data1 |= 1;
          break;
        case IECODE_LBUTTON | IECODE_UP_PREFIX:
          joyevent.data1 &= ~1;
          break;
        case IECODE_RBUTTON:
          joyevent.data1 |= 2;
          break;
        case IECODE_RBUTTON | IECODE_UP_PREFIX:
          joyevent.data1 &= ~2;
          break;
        case IECODE_MBUTTON:
          joyevent.data1 |= 4;
          break;
        case IECODE_MBUTTON | IECODE_UP_PREFIX:
          joyevent.data1 &= ~4;
          break;
        case IECODE_NOBUTTON:
          joyevent.data2 = gameport_ie.ie_X;
          joyevent.data3 = gameport_ie.ie_Y;
          break;
        default:
          break;
      }
      joyevent.type = ev_joystick;
      D_PostEvent (&joyevent);
      gameport_io->io_Command = GPD_READEVENT;
      gameport_io->io_Length = sizeof (struct InputEvent);
      gameport_io->io_Data = &gameport_ie;
      SendIO ((struct IORequest *)gameport_io);
    }
  }

  /* CD32 joypad handler code supplied by Gabry (ggreco@iol.it) */

  if (LowLevelBase != NULL) {
    event_t joyevent;
    ULONG joypos = ReadJoyPort (1);

    if (previous == joypos)
      return;

    joyevent.type = ev_joystick;
    joyevent.data1 = joyevent.data2 = joyevent.data3 = 0;

    if (joypos & JPF_BUTTON_RED)
      joyevent.data1 |= 1;
    else
      joyevent.data1 &= ~1;

    if (joypos & JP_DIRECTION_MASK) {
      if (joypos & JPF_JOY_LEFT) {
        joyevent.data2 = -1;
      } else if (joypos & JPF_JOY_RIGHT) {
        joyevent.data2 = 1;
      }
      if (joypos & JPF_JOY_UP) {
        joyevent.data3 = -1;
      } else if (joypos & JPF_JOY_DOWN) {
        joyevent.data3 = 1;
      }
    }

    if (joypos & JP_TYPE_GAMECTLR) {
      event_t event;

      // Play/Pause = ESC (Menu)
      if (joypos & JPF_BUTTON_PLAY && !(previous & JPF_BUTTON_PLAY)) {
        event.type = ev_keydown;
        event.data1 = KEY_ESCAPE;
        D_PostEvent (&event);
      } else if (previous & JPF_BUTTON_PLAY) {
        event.type = ev_keyup;
        event.data1 = KEY_ESCAPE;
        D_PostEvent (&event);
      }

      // YELLOW = SHIFT (button 2) (Run)
      if (joypos & JPF_BUTTON_YELLOW)
        joyevent.data1 |= 4;
      else
        joyevent.data1 &= ~4;

      // BLUE = SPACE (button 3) (Open/Operate)

      if (joypos & JPF_BUTTON_BLUE)
        joyevent.data1 |= 8;
      else
        joyevent.data1 &= ~8;

      // GREEN = RETURN (show msg)

      if (joypos & JPF_BUTTON_GREEN && !(previous&JPF_BUTTON_GREEN)) {
        event.type = ev_keydown;
        event.data1 = 13;
        D_PostEvent (&event);
      } else if (previous & JPF_BUTTON_GREEN) {
        event.type = ev_keyup;
        event.data1 = 13;
        D_PostEvent (&event);
      }

      // FORWARD & REVERSE - ALT (Button1) Strafe left/right

      if (joypos & JPF_BUTTON_FORWARD) {
        joyevent.data1 |= 2;
        joyevent.data2 = 1;
      } else if (joypos & JPF_BUTTON_REVERSE) {
        joyevent.data1 |=2;
        joyevent.data2=-1;
      } else
        joyevent.data1 &= ~2;
    }

    D_PostEvent (&joyevent);

    previous = joypos;
  }

  /* SEGA joypad handler code by Joe Fenton, loosely based on CD32 handling */

  if (sega3_selected || sega6_selected) {
    event_t joyevent, event;

    if (sega3_selected) {
      currSega = Sega3 ();
    } else {
      currSega = Sega6 ();
    }

    if (prevSega == currSega)
      return;

    joyevent.type = ev_joystick;
    joyevent.data1 = joyevent.data2 = joyevent.data3 = 0;

    // B = fire
    if (currSega & 0x10)
      joyevent.data1 |= 1;
    else
      joyevent.data1 &= ~1;

    // directionals
    if (currSega & 0xF) {
      if (currSega & 4) {
        joyevent.data2 = -1;
      } else if (currSega & 8) {
        joyevent.data2 = 1;
      }
      if (currSega & 1) {
        joyevent.data3 = -1;
      } else if (currSega & 2) {
        joyevent.data3 = 1;
      }
    }

    // Mode = ESC (Menu)
    if (currSega & 0x80000 && !(prevSega & 0x80000)) {
      event.type = ev_keydown;
      event.data1 = KEY_ESCAPE;
      D_PostEvent (&event);
    } else if (prevSega & 0x80000) {
      event.type = ev_keyup;
      event.data1 = KEY_ESCAPE;
      D_PostEvent (&event);
    }

    // Y = SHIFT (Run)
    if (currSega & 0x20000)
      joyevent.data1 |= 4;
    else
      joyevent.data1 &= ~4;

    // Start = SPACE (Open/Operate)
    if (currSega & 0x2000)
      joyevent.data1 |= 8;
    else
      joyevent.data1 &= ~8;

    // A & C - ALT (Button1) Strafe left/right
    if (currSega & 0x20) {
      joyevent.data1 |= 2;
      joyevent.data2 = 1;
    } else if (currSega & 0x1000) {
      joyevent.data1 |=2;
      joyevent.data2=-1;
    } else
      joyevent.data1 &= ~2;

    // X = RETURN (show msg)
    if (currSega & 0x40000 && !(prevSega&0x40000)) {
      event.type = ev_keydown;
      event.data1 = 13;
      D_PostEvent (&event);
    } else if (prevSega & 0x40000) {
      event.type = ev_keyup;
      event.data1 = 13;
      D_PostEvent (&event);
    }

    // Z = TAB (show map)
    if (currSega & 0x10000 && !(prevSega&0x10000)) {
      event.type = ev_keydown;
      event.data1 = 9;
      D_PostEvent (&event);
    } else if (prevSega & 0x10000) {
      event.type = ev_keyup;
      event.data1 = 9;
      D_PostEvent (&event);
    }

    D_PostEvent (&joyevent);

    prevSega = currSega;
  }
#endif
}

/**********************************************************************/
static void calc_time (ULONG time, char *msg)
{
  printf ("Total %s = %u us  (%u us/frame)\n", msg, time, time / total_frames);
}

/**********************************************************************/
void _STDvideo_cleanup (void)
{
  I_ShutdownGraphics ();
  if (video_smr != NULL) {
    FreeAslRequest (video_smr);
    video_smr = NULL;
  }
  if (total_frames > 0) {
    printf ("Total number of frames = %u\n", total_frames);
    calc_time (wpa8_time, "WritePixelArray8 time ");
    calc_time (lock_time, "LockBitMap time       ");
    calc_time (c2p_time, "Chunky2Planar time    ");
#ifdef PROFILE
    {
      int i;

      for (i=0; i<32; i++)
        calc_time (profile[n][2], "Profile Time ");
    }
#endif
  }
}

/**********************************************************************/
