  /*
  * UAE - The Un*x Amiga Emulator
  *
  * AROS Interface
  *
  * Copyright © 2003 Fabio Alemagna.
  */


#include "sysconfig.h"
#include "sysdeps.h"

/****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <aros/autoinit.h>
#include <exec/types.h>
#include <dos/dos.h>

#define DEBUG 1
#include <aros/debug.h>

#include "aros-gui.h"

/****************************************************************************/

#undef Exception  /* Undef Exception because otherwise would conflict with the function
                   * with the same name used in UAE
				   */
/****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "options.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "xwin.h"
#include "drawing.h"
#include "keyboard.h"
#include "keybuf.h"
#include "gui.h"
#include "debug.h"
#include "disk.h"


#define BitMap Picasso96BitMap  /* Argh! */
#include "picasso96.h"
#undef BitMap

/****************************************************************************/

extern xcolnr xcolors[4096];

 /* Keyboard and mouse */

static int  keystate[256];
static int  screen_is_picasso;
static char picasso_invalid_lines[1200];
static int  picasso_invalid_start;
static int  picasso_invalid_end;

extern int buttonstate[3];
extern int lastmx, lastmy;
extern int newmousecounters;

static void uaedisplay_eventhandler(struct IntuiMessage *imsg);
static void set_screen_for_picasso(void);

void flush_line(int y)
{
    printf("Shouldn't have arrived here\n");
    abort();
}

void flush_block (int ystart, int ystop)
{
    DoMethod(uaedisplay, MUIM_UAEDisplay_Update, ystart, ystop);
}

void flush_screen (int ystart, int ystop)
{
/* WaitBOVP() ? */
}

static int init_colors(int depth)
{
    switch(depth) {
      case 15:
        alloc_colors64k(5,5,5,10,5,0);
        break;

      case 16:
        alloc_colors64k(5,6,5,11,5,0);
        break;

      case 24:
        alloc_colors64k(8,8,8,16,8,0);
        break;

      case 32:
        alloc_colors64k(8,8,8,16,8,0);
        break;

      default:
	printf("Shouln't have arrived here\n");
	abort();
    }

    printf("Using %d bits truecolor.\n", depth);

    return 1;
}


int graphics_setup(void)
{
    return 1;
}

int graphics_init(void)
{
    int i,bitdepth;

    if (currprefs.color_mode > 5)
        fprintf(stderr, "Bad color mode selected. Using default.\n"), currprefs.color_mode = 0;

    fixup_prefs_dimensions (&currprefs);

    if
    (
       !SetAttrs
        (
            uaedisplay,
            MUIA_UAEDisplay_Width,        currprefs.gfx_width,
	    MUIA_UAEDisplay_Height,       currprefs.gfx_height,
	    MUIA_UAEDisplay_EventHandler, (IPTR)uaedisplay_eventhandler,
	    TAG_DONE
        )
    )
    {
        __showerror("Failed to SetAttrs() on uaedisplay(%lp - %lp)", uaedisplay, &uaedisplay);
        return 0;
    }
    
    gfxvidinfo.emergmem = 0;
    gfxvidinfo.linemem = 0;

    gfxvidinfo.rowbytes = XGET(uaedisplay, MUIA_UAEDisplay_BytesPerRow);
    gfxvidinfo.pixbytes = XGET(uaedisplay, MUIA_UAEDisplay_BytesPerPix);
    gfxvidinfo.width    = XGET(uaedisplay, MUIA_UAEDisplay_Width);
    gfxvidinfo.height   = XGET(uaedisplay, MUIA_UAEDisplay_Height);
    gfxvidinfo.bufmem   = (void *)XGET(uaedisplay, MUIA_UAEDisplay_Memory);

    gfxvidinfo.maxblocklines = currprefs.gfx_height-1;

    if (!init_colors(XGET(uaedisplay, MUIA_UAEDisplay_BitDepth))) {
        fprintf(stderr,"Failed to init colors.\n");
        return 0;
    }

    switch (gfxvidinfo.pixbytes) {
     case 2:
        for (i = 0; i < 4096; i++) xcolors[i] *= 0x00010001;
        gfxvidinfo.can_double = 1;
        break;
     case 1:
        for (i = 0; i < 4096; i++) xcolors[i] *= 0x01010101;
        gfxvidinfo.can_double = 1;
        break;
     default:
        gfxvidinfo.can_double = 0;
        break;
    }

    buttonstate[0] = buttonstate[1] = buttonstate[2] = 0;
    memset(keystate, 0, sizeof(keystate));

    lastmx = lastmy = 0;
    newmousecounters = 0;

    return 1;
}

/****************************************************************************/

int check_prefs_changed_gfx (void)
{
	return 0;
}

/****************************************************************************/

void graphics_leave(void)
{
}

/****************************************************************************/


void uaedisplay_eventhandler(struct IntuiMessage *imsg)
{
    switch(imsg->Class)
    {
            //toggle_inhibit_frame((_window(uaedisplay)->Flags & WFLG_ZOOMED)?IHF_WINDOWZOOM:0);
//            break;

        case IDCMP_RAWKEY:
	{
            int kc       = imsg->Code&127;
            int released = imsg->Code&128?1:0;

            if(released)
	    {
                keystate[kc] = 0;
                record_key ((kc << 1) | 1);
            }
	    else
	    if (!keystate[kc])
	    {
                keystate[kc] = 1;
                record_key (kc << 1);
            }

	    break;
        }

        case IDCMP_ACTIVEWINDOW:
            newmousecounters = 1;
	   /* fall trough */

        case IDCMP_MOUSEMOVE:
	    lastmx = imsg->MouseX;
            lastmy = imsg->MouseY;
	    break;

       case IDCMP_MOUSEBUTTONS:
            if (imsg->Code == SELECTDOWN) buttonstate[0]=1;
            if (imsg->Code == SELECTUP)   buttonstate[0]=0;
            if (imsg->Code == MIDDLEDOWN) buttonstate[2]=1;
            if (imsg->Code == MIDDLEUP)   buttonstate[2]=0;
            if (imsg->Code == MENUDOWN)   buttonstate[2]=1;
            if (imsg->Code == MENUUP)     buttonstate[2]=0;
            break;

       default:
            fprintf(stderr, "Unknown class: %d\n", imsg->Class);
            break;
    }
#if 0
    /* "Affengriff" */
    if(keystate[AK_CTRL] && keystate[AK_LAMI] && keystate[AK_RAMI])
        m68k_reset();

    /* PC-like :-) CTRL-ALT-DEL => reboot */
    if(keystate[AK_CTRL] && (keystate[AK_LALT] || keystate[AK_RALT]) &&
       keystate[AK_DEL])
        m68k_reset();

    /* CTRL+LSHIFT+LALT+F10 on amiga => F12 on X11 */
    /*                  F9           => ScrollLock on X11 (inhibit_frame) */
    if(keystate[AK_CTRL] && keystate[AK_LSH] && keystate[AK_LALT]) {
/*        if(keystate[AK_F10]) togglemouse(); */
        if(keystate[AK_F8]) {
            uae_quit();
        }
	/*
	if(keystate[AK_F9]) {
            do_inhibit_frame(inhibit_frame != 2 ? inhibit_frame ^ 1 :
                            inhibit_frame);
            DisplayBeep(NULL);
        } */
    }

    disk_hotkeys();
 #endif
}

void handle_events(void)
{
    newmousecounters = 0;

    #ifdef PICASSO96
    if (screen_is_picasso)
    {
        int i;

        picasso_invalid_lines[picasso_invalid_end+1] = 0;
        picasso_invalid_lines[picasso_invalid_end+2] = 1;

	for (i = picasso_invalid_start; i < picasso_invalid_end;)
	{
	    int start = i;

            for (;picasso_invalid_lines[i]; i++)
	        picasso_invalid_lines[i] = 0;

            DoMethod(uaedisplay, MUIM_UAEDisplay_Update, start, i);

	    for (; !picasso_invalid_lines[i]; i++);
	}

	picasso_invalid_start = picasso_vidinfo.height + 1;
	picasso_invalid_end   = -1;
    }
    #endif

    gui_handle_events();
}

/***************************************************************************/

int debuggable(void)
{
    return 1;
}

/***************************************************************************/

int needmousehack(void)
{
    return 1;
}

/***************************************************************************/

void LED(int on)
{
}

/***************************************************************************/

#ifdef PICASSO96

void DX_Invalidate (int first, int last)
{
    if (first < picasso_invalid_start)
        picasso_invalid_start = first;

    if (last > picasso_invalid_end)
        picasso_invalid_end = last;

    memset(&picasso_invalid_lines[first], 1, last-first+1);
}

int DX_BitsPerCannon (void)
{
    D(bug("DX_BitsPerCannon()\n"));
    return 8;
}

void DX_SetPalette(int start, int count)
{
    D(bug("DX_SetPalette(%d, %d)\n", start, count));
    if (!screen_is_picasso || picasso_vidinfo.pixbytes != 1)
	return;

    printf("DX_SetPalette(): aborting()\n");
    abort();
}

int DX_FillResolutions (uae_u16 *ppixel_format)
{
    D(bug("DX_FillResolutions()\n"));
    int count = 0;
    ULONG j;

    static struct
    {
        int width, height;
    } modes [] =
    {
        { 320,  200 },
	{ 320,  240 },
	{ 320,  256 },
	{ 640,  200 },
	{ 640,  240 },
	{ 640,  256 },
	{ 640,  400 },
	{ 640,  480 },
	{ 640,  512 },
	{ 800,  600 },
	{ 1024, 768 }
    };

    int bpx  = XGET(uaedisplay, MUIA_UAEDisplay_BytesPerPix);
    int maxw = XGET(uaedisplay, MUIA_UAEDisplay_MaxWidth);
    int maxh = XGET(uaedisplay, MUIA_UAEDisplay_MaxHeight);

    D(bug("maxheight = %d\n", maxh));
    D(bug("maxwidth  = %d\n", maxw));

    for (j = 0; (j < (sizeof(modes)/sizeof(modes[0]))) && (j < MAX_PICASSO_MODES); j++)
    {
        static const int bpx2format[] = {0, RGBFF_CHUNKY, RGBFF_R5G6B5PC, 0, RGBFF_B8G8R8A8};

	if (modes[j].width > maxw || modes[j].height > maxh)
	    continue;

	DisplayModes[count].res.width  = modes[j].width;
        DisplayModes[count].res.height = modes[j].height;
        DisplayModes[count].depth      = bpx;
        DisplayModes[count].refresh    = 75;

        count++;
        *ppixel_format |= bpx2format[bpx];
    }

    return count;
}

void gfx_set_picasso_modeinfo (int w, int h, int depth, int rgbfmt)
{
    D(bug("gfx_set_picasso_modeinfo(%d ,%d, %d, %d)\n", w, h, depth, rgbfmt));
    picasso_vidinfo.width = w;
    picasso_vidinfo.height = h;
    picasso_vidinfo.depth = depth;
    picasso_vidinfo.rgbformat = (depth == 8 ? RGBFB_CHUNKY
				 : depth == 16 ? RGBFB_R5G6B5PC
				 : RGBFB_B8G8R8A8);

    if (screen_is_picasso)
        set_screen_for_picasso();
}

void gfx_set_picasso_baseaddr (uaecptr a)
{
    D(bug("gfx_set_picasso_baseaddr(0x%08lx)\n", a));
}

static APTR picasso_memory;

static void set_screen_for_picasso(void)
{
    if
    (
       !SetAttrs
        (
            uaedisplay,
            MUIA_UAEDisplay_Width,  picasso_vidinfo.width,
	    MUIA_UAEDisplay_Height, picasso_vidinfo.height,
	    TAG_DONE
        )
    )
    {
        bug ("SetAttrs - 1 - aborting()\n");
	abort();
    };

    picasso_vidinfo.extra_mem = 1;
    picasso_vidinfo.rowbytes = XGET(uaedisplay, MUIA_UAEDisplay_BytesPerRow);
    picasso_vidinfo.pixbytes = XGET(uaedisplay, MUIA_UAEDisplay_BytesPerPix);
    picasso_memory           = (APTR)XGET(uaedisplay, MUIA_UAEDisplay_Memory);

    picasso_invalid_start = picasso_vidinfo.height + 1;
    picasso_invalid_end   = -1;

    kprintf
    (
	"Width:  %d\n"
	"Height: %d\n"
	"BPP:    %d\n"
	"BPR:    %d\n",
        XGET(uaedisplay, MUIA_UAEDisplay_Width),
        XGET(uaedisplay, MUIA_UAEDisplay_Height),
        XGET(uaedisplay, MUIA_UAEDisplay_BytesPerPix),
        XGET(uaedisplay, MUIA_UAEDisplay_BytesPerRow)
    );
}

void gfx_set_picasso_state (int on)
{
    D(bug("gfx_set_picasso_state(%d)\n", on));

    if (screen_is_picasso == on )
	return;

    screen_is_picasso = on;
    if (on)
    {
        set_screen_for_picasso();
    }
    else
    {
        if
	(
	   !SetAttrs
            (
                uaedisplay,
                MUIA_UAEDisplay_Width,  currprefs.gfx_width,
	        MUIA_UAEDisplay_Height, currprefs.gfx_height,
	        TAG_DONE
            )
	)
	{
            bug ("SetAttrs - 2 - aborting()\n");
	    abort();
	};

	gfxvidinfo.bufmem = (APTR)XGET(uaedisplay, MUIA_UAEDisplay_Memory);
    }
}


uae_u8 *gfx_lock_picasso (void)
{
//    D(bug("gfx_lock_picasso()\n"));
    return picasso_memory;
}
void gfx_unlock_picasso (void)
{
//    D(bug("gfx_unlock_picasso()\n"));
}

#endif


int lockscr (void)
{
return 1;
}

void unlockscr (void)
{
}


/***************************************************************************/

static int led_state[5];

/****************************************************************************/

/****************************************************************************/

static void unrecord(int kc)
{
    keystate[kc] = 0;
    record_key ((kc << 1) | 1);
}

/****************************************************************************/

static void disk_hotkeys(void)
{
    struct FileRequester *FileRequest;
    char buff[80];
    int drive;
    char *last_file,*last_dir,*s;

    if(!(keystate[AK_CTRL] && keystate[AK_LALT])) return;

    /* CTRL-LSHIFT-LALT F1-F4 => eject_disk */
    if(keystate[AK_LSH]) {
        int ok = 0;

        if(keystate[AK_F1]) {ok=1;disk_eject(0);
                             printf("drive DF0: ejected\n");}
        if(keystate[AK_F2]) {ok=1;disk_eject(1);
                             printf("drive DF1: ejected\n");}
        if(keystate[AK_F3]) {ok=1;disk_eject(2);
                             printf("drive DF2: ejected\n");}
        if(keystate[AK_F4]) {ok=1;disk_eject(3);
                             printf("drive DF3: ejected\n");}

        if(ok) {
            DisplayBeep(NULL);
            unrecord(AK_CTRL);unrecord(AK_LALT);unrecord(AK_LSH);
            unrecord(AK_F1);unrecord(AK_F2);
            unrecord(AK_F3);unrecord(AK_F4);
        }
        return;
    }

    /* CTRL-LALT F1-F4 => insert_disk */
    if(keystate[AK_F1])      {drive = 0;unrecord(AK_F1);}
    else if(keystate[AK_F2]) {drive = 1;unrecord(AK_F2);}
    else if(keystate[AK_F3]) {drive = 2;unrecord(AK_F3);}
    else if(keystate[AK_F4]) {drive = 3;unrecord(AK_F4);}
    else return;
    DisplayBeep(NULL);
    unrecord(AK_CTRL);unrecord(AK_LALT);

    switch(drive) {
      case 0: case 1: case 2: case 3: last_file = currprefs.df[drive]; break;
      default: return;
    }

    if(!last_file) return;
    if(!last_dir)  return;

}

/****************************************************************************/

void target_save_options (FILE *f, struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, char *option, char *value)
{
	return 0;
}

void setup_brkhandler(void)
{}
