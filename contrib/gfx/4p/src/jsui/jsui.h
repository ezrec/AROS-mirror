/*
 *                       ________              __
 *                      /\___  __\            /\_\
 *                      \/__/\ \_/  ____ __  _\/_/_
 *                          \ \ \  / __//\ \|\ \/\ \
 *                         __\_\ \/\___`\ \ \_\ \ \ \
 *                        /\_____/\/|___/\ \____/\ \_\
 *                        \/____/  |/__/  \/___/  \/_/
 *
 * jsui
 * 
 *   The main API for the JSUI library
 *   Logo idea blatantly stolen from the Allegro library by Sean Hargreaves.
 */

/*
 * JSUI - Jerry's SDL User Interface
 * Copyright (C) 2001 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 *  This is primarilly based on the ALLEGRO user intreface, which 
 *  is based on the Atari's TOS interface.  I've decided to go with
 *  this because it seems full featured while being simple..
 *  ... oh, and I've worked with ALLEGRO for a few years, and I'm
 *  comfortable with this design.
 *
 *  Some things were changed to make it less drain bramaged.
 */

#include <SDL.h>

#ifndef __JSUI_H__
#define __JSUI_H__

/****************************************************************************
 ****************************************************************************
 **************      Base drawing elements. (pixel, pen, page)
 */


/*    pixel   */
typedef struct __colorelement {
    int r;
    int g;
    int b;
} COLEL;



/*    pen   */
typedef struct __pen
{
#if 0
    long masktype;      // 0x00 plot pixel.
                        // 0x02 plot xor of background
                        // 0x04 plot only in the alpha channel
    long transparency;  // 0 to 255.  255 is 100% on.  (Truecolor only)
#endif

    int Icolor;		// the color to use. (index)

    COLEL RGBcolor;	// the color to use. (RGB)
    long text_type;     // see below
} PEN;


#define TEXT_TYPE_JAM1  (0x01)    /* fg = color, bg = transparent */
#define TEXT_TYPE_JAM2  (0x02)    /* fg = color, bg = bgcolor */


PEN * Pen_Create  (void);
void  Pen_Destroy (PEN * pen);


/*    page   */

/*
 * a page is an image that is loaded into the program, or is created from
 * scratch.  features of a 'page' are:
 *	- undo buffer/queue
 *      - regions changed since the undo buffer
 *      - its own palette
 */


// color ranges
typedef struct __color_range {
    int min;
    int max;
    double delay;
} COLOR_RANGE;


// image pages
typedef struct __image_page {
    unsigned long id;	// unique ID for every page

    int w;		// width
    int h;		// height

    // the pen to do 'drawings' with..
    PEN * fgpen;	// foreground
    PEN * bgpen;	// background

    // for truecolor images...
    COLEL * timage;	// image data

    // for paletted images...
    int ncolors;	// number of colors
    int * pimage;	// image data
    COLEL * palette;	// the palette
    int pdirty;		// the palette is dirty

    // for undo checkpointing

    COLEL * undo_timage;	// TIMMAHH!!!
    int   * undo_pimage;
    COLEL * undo_palette;

    // for both, we'll have an alpha channel
    int * alpha;

    // and for the dirty rectangles, well store the region here:
    // if there's nothing to update, set all of them to -1. 
    // (this will get done in the view handler)
    int dr_x1, dr_y1;
    int dr_x2, dr_y2;

    // in case it's a brush, we'll need the handle coordinates..  
    // these are an offset from the upper right of the image.
    int handle_x;
    int handle_y;

    // and of course, for color cycling, we need to have lists of cycle colors
    int nranges;
    COLOR_RANGE * color_cycle;
} IPAGE;


extern IPAGE * jsui_screen;
extern IPAGE * jsui_active_background_page;  // backing page, supplied externally

extern SDL_Surface *actual_screen;

//  ncolors == 0 if truecolor
IPAGE * Page_Create(int w, int h, int ncolors);
void    Page_Destroy(IPAGE * page);
IPAGE * Page_Duplicate(IPAGE * page);


/////////////////////////////////////////////
void Page_DR_Dirtify(IPAGE * page, int rx1, int ry1, int rx2, int ry2);
void Page_DR_Clean(IPAGE * page);

/////////////////////////////////////////////

// for the rubberband layer.  negative areas are transparent
void Page_Negative(IPAGE * page);

/////////////////////////////////////////////
// this should probably be an enum.  oh well.
#define DRAW_STYLE_NONE		(0)
#define DRAW_STYLE_MATTE	(1)	/* use the alpha channel */
#define DRAW_STYLE_COLOR	(2)	/* just paint the fg color */
#define DRAW_STYLE_FGCOLOR	(3)	/* just paint the fg color */
#define DRAW_STYLE_BGCOLOR	(4)	/* just paint the fg color */
#define DRAW_STYLE_REPLACE	(5)	/* paint the entire image, no alpha */
#define DRAW_STYLE_SMEAR	(6)	/* smears the pixels underneath it */
#define DRAW_STYLE_SHADE	(7)	/* increases/decreases pixel values */
#define DRAW_STYLE_BLEND	(8)	/* average together pixels ??? */
#define DRAW_STYLE_CYCLE	(9)	/* draw, cycling through colors */
#define DRAW_STYLE_SMOOTH	(10)	/* blurs the image underneath */
#define DRAW_STYLE_TINT		(11)	/* shade the image with the color */

void Page_Blit_Image(
	IPAGE * destination,
        int x, int y,
        IPAGE * source,
        int x1, int y1,
        int x2, int y2
);

void Page_DoubleBlit_Image(
	IPAGE * destination,
        int x, int y,
        IPAGE * source,
        int x1, int y1,
        int x2, int y2
);

void
Page_VariBlit_Image(
	IPAGE * destination, 
	int idx, int idy, 
	IPAGE * source,
	int isx, int isy,
        int *skips, int *widths,
        int maxw, int h
);

void Page_Draw_Image(
	IPAGE * destination,
        int x, int y,
        IPAGE * source,
        int x1, int y1,
        int x2, int y2,
	int draw_mode
);

#define PAGE_DRAW_BRUSH(d,x,y, s,m)\
		Page_Draw_Image((d),\
			(x) - (s)->handle_x,\
			(y) - (s)->handle_y,\
			(s), 0, 0, (s)->w, (s)->h, (m))

void Page_Copy_Palette(IPAGE * destination, IPAGE * source);

void Page_Copy_Pens(IPAGE * destination, IPAGE * source);


void Page_Alpha_Fill(IPAGE * page, int set_to);

void Page_Alpha_Chromakey_Index(IPAGE * page, int key, int set_to);
void Page_Alpha_Chromakey_RGB(IPAGE * page, COLEL key, int set_to);

void Page_Handle_Center(IPAGE * page);

IPAGE * Page_Cutout_Brush(IPAGE * page, int x1, int y1, int x2, int y2,
			    int excl_brush, PEN * wipe_original);

void Page_Undo_Checkpoint(IPAGE * page);
void Page_Undo(IPAGE * page);


void Page_Flip_Horizontal(IPAGE * page);
void Page_Flip_Vertical(IPAGE * page);

void Page_Swap_XY(IPAGE * page);


void Page_Halve_Size(IPAGE * page);

void Page_Double_Horizontal(IPAGE * page);
void Page_Double_Vertical(IPAGE * page);

void Page_Halve_Size(IPAGE * page);


/****************************************************************************
 ****************************************************************************
 **************      graphics base
 */


void gfx_screen_blit(SDL_Surface * screen, IPAGE * base_page);

//void gfx_screen_blit_2x(SDL_Surface * screen,
//			IPAGE * rubberband_page, IPAGE * base_page);


/****************************************************************************
 ****************************************************************************
 **************      graphics primitives
 */


typedef void (*primitive_callback)
(
    IPAGE * page,
    PEN * pen,
    int x,
    int y
);


void
primitive_pixel(
    IPAGE * page,
    PEN * pen,
    int x,
    int y
);

void
__primitive_pixel(
    IPAGE * page,
    PEN * pen,
    int x,
    int y
);

void
primitive_pixel_dotted(
    IPAGE * page,
    PEN * pen,
    int x,
    int y
);


int
primitive_pixel_get(
    IPAGE * page,
    int x, 
    int y
);

void
primitive_line_horizontal(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int x2,
    int y
);

void
primitive_line_vertical(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x,
    int y1,
    int y2
);

void
primitive_line(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int y1,
    int x2,
    int y2
);

void
primitive_rectangle_hollow(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int y1,
    int x2,
    int y2
);

void
primitive_rectangle_filled(
    IPAGE * page,
    PEN * pen,
    int x1,
    int y1,
    int x2,
    int y2
);

void    
primitive_rectangle_checkerboard(
        IPAGE * dest,
        int x1, int y1, int x2, int y2,
        int fillcolor
);   



void
primitive_rectangle_fourcolor(
        IPAGE * dest,
        primitive_callback func,
        int x1, int y1, int x2, int y2,
        int fillcolor,
        int c1, int c2, int c3, int c4
);

#define primitive_3d_rect_down(b,x1,y1,x2,y2,f) \
	primitive_rectangle_fourcolor((b), primitive_pixel, \
	(x1), (y1), (x2), (y2), (f), \
	JSUI_C_DARK, JSUI_C_BRIGHT, JSUI_C_BRIGHT, JSUI_C_DARK);


#define primitive_3d_rect_up(b,x1,y1,x2,y2,f) \
	primitive_rectangle_fourcolor((b), primitive_pixel, \
	(x1), (y1), (x2), (y2), (f), \
	JSUI_C_BRIGHT, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_BRIGHT);


void
primitive_circle_hollow(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x,
    int y,
    double radius
);


void
primitive_floodfill(
        IPAGE * dest,
        PEN * pen,
        int x, int y
);



/****************************************************************************
 ****************************************************************************
 **************      Font
 */


typedef struct __font_definition{
    int w;                      // width of each characters
    int h;                      // height of each character
    int min_ascii;              // minimum ascii value
    int max_ascii;              // maximum ascii value
    unsigned char * data;       // pointer to the data
} FONT_DEFINITION;

extern FONT_DEFINITION jsui_standard_font;

extern FONT_DEFINITION * jsui_current_font;

void
font_render_text(
	IPAGE * dest, 
	PEN * fgpen, 
	PEN * bgpen, 
	FONT_DEFINITION * font,
	int x, int y, 
	char * text
);

void
font_render_text_center(
	IPAGE * dest, 
	PEN * fgpen, 
	PEN * bgpen, 
	FONT_DEFINITION * font,
	int x, int y, 
	char * text
);

void
font_render_text_right(
	IPAGE * dest, 
	PEN * fgpen, 
	PEN * bgpen, 
	FONT_DEFINITION * font,
	int x, int y, 
	char * text
);



/****************************************************************************
 ****************************************************************************
 **************      Palette
 */


extern int __pal32_1[]; // Deluxe Paint 1  32 color palette
extern int __pal32_2[]; // Deluxe Paint 2  32 color palette

void palette_free(COLEL * palette);

COLEL * palette_generate_default(int ncolors);


/****************************************************************************
 ****************************************************************************
 **************      Timing
 */

extern unsigned long timing_ticks;


/****************************************************************************
 ****************************************************************************
 **************      Cursor Control
 */

/* Is the cursor visible? */
extern int cursor_visible;


/****************************************************************************
 ****************************************************************************
 **************      GUI
 */

/* bits for the flags field */
#define JSUI_F_EXIT       0x0001  /* object makes the dialog exit */
#define JSUI_F_SELECTED   0x0002  /* object is selected */
#define JSUI_F_GOTFOCUS   0x0004  /* object has the input focus */
#define JSUI_F_GOTMOUSE   0x0008  /* mouse is on top of object */
#define JSUI_F_HIDDEN     0x0010  /* object is not visible */
#define JSUI_F_DISABLED   0x0020  /* object is visible but inactive */
#define JSUI_F_DIRTY      0x0040  /* object needs to be redrawn */
#define JSUI_F_NOFRAME    0x0080  /* object has no frame*/
#define JSUI_F_CLEAR      0x0100  /* object has no filler */
#define JSUI_F_ZLOCK      0x0200  /* object does not get raised or lowered */
#define JSUI_F_FLIP       0x0400  /* object gets rendered backwards from norm */
#define JSUI_F_MODAL      0x0800  /* (frame) locks focus */
#define JSUI_F_NOFOCUS    0x1000  /* widget can never get focus */
#define JSUI_F_NODRIVE    0x1000
#define JSUI_F_NOAMBITION 0x1000 
#define JSUI_F_NOFOCUSBOX 0x2000  /* no focus box appears in the widget */
#define JSUI_F_USER       0x4000  /* from here on is free for your use */


/* return values for the dialog procedures */
#define JSUI_R_O_K         0x00   /* normal exit status */
#define JSUI_R_CLOSE       0x01   /* request to close the dialog */
#define JSUI_R_REDRAW      0x02   /* request to redraw the dialog */
#define JSUI_R_REDRAWME    0x04   /* request to redraw this object */
#define JSUI_R_WANTFOCUS   0x08   /* this object wants the input focus */
#define JSUI_R_USED_CHAR   0x10   /* object has used the keypress */


/* messages for the dialog procedures */
#define JSUI_MSG_START       1    /* start the dialog, initialise */
#define JSUI_MSG_END         2    /* dialog is finished - cleanup */
#define JSUI_MSG_DRAW        3    /* draw the object */
#define JSUI_MSG_GOTFOCUS    4    /* got the input focus */
#define JSUI_MSG_LOSTFOCUS   5    /* lost the input focus */
#define JSUI_MSG_LPRESS      6    /* left button pressed */
#define JSUI_MSG_MPRESS      7    /* middle button pressed */
#define JSUI_MSG_RPRESS      8    /* right button pressed */
#define JSUI_MSG_LDRAG       9    /* left button drag on the object */
#define JSUI_MSG_MDRAG       10   /* middle button drag on the object */
#define JSUI_MSG_RDRAG       11   /* right button drag on the object */
#define JSUI_MSG_LRELEASE    12   /* left button released */
#define JSUI_MSG_MRELEASE    13   /* middle button released */
#define JSUI_MSG_RRELEASE    14   /* right button released */
#define JSUI_MSG_MOUSEMOVE   15   /* mouse moved */

#define JSUI_MSG_LDCLICK     16   /* left double click on the object */
#define JSUI_MSG_MDCLICK     17   /* middle double click on the object */
#define JSUI_MSG_RDCLICK     18   /* right double click on the object */
#define JSUI_MSG_KEY         19   /* keyboard shortcut */
#define JSUI_MSG_CHAR        20   /* other keyboard input */
#define JSUI_MSG_UCHAR       21   /* unicode keyboard input */
#define JSUI_MSG_XCHAR       22   /* broadcast character to all objects */
#define JSUI_MSG_WANTFOCUS   23   /* does object want the input focus? */

#define JSUI_MSG_IDLE        24   /* update any background stuff */
#define JSUI_MSG_RADIO       25   /* clear radio buttons */
#define JSUI_MSG_TIMETICK    26   /* one tick of time has passed */

#define JSUI_MSG_TRIGGER_L   27   /* Trigger as if it was pressed with left */
#define JSUI_MSG_TRIGGER_R   28   /* Trigger as if it was pressed with right */

#define JSUI_MSG_USER       1024  /* from here on are free... */



/************************************************************
 * Structures
 */

/* dialog elements */
typedef struct JSUI_DIALOG
{
    int (*proc)(int, void *, struct JSUI_DIALOG *, int); 
				  /* dialog procedure 
					int message
					JDR * jdr
					DLG * dlg
					int c
				  */
    int x, y, w, h;               /* position and size of the object */
    int key;                      /* keyboard shortcut (ASCII code) */
    int flags;                    /* flags about the object state */
    int d1, d2;                   /* any data the object might require */
    void *dp, *dp2, *dp3;         /* pointers to more object data */
} JSUI_DIALOG;


typedef struct NEW_JSUI_DIALOG
{
    int (*proc)(int, void *, struct JSUI_DIALOG *, int); 
				  /* dialog procedure 
					int message
					JDR * jdr
					DLG * dlg
					int c
				  */
    int x, y, w, h;               /* position and size of the object */
    int key;                      /* keyboard shortcut (ASCII code) */
    int jflags;                   /* general flags about the object state */
    int oflags;                   /* flags specific to this object */
    int d1, d2;                   /* any data the object might require */
    void *dp, *dp2;               /* pointers to more object data */
    void *ud;			  /* user data */
    char *t1, *t2;		  /* text pointers? */
    void *cb;			  /* callback */
} NEW_JSUI_DIALOG;

#define JSUI_DLG_END   { NULL, 0, 0, 0, 0,  0, 0, 0, 0, NULL, NULL, NULL  }



/* popup menus */
typedef struct JSUI_MENU
{
    char *text;                   /* menu item text */
    int (*proc)(int, struct JSUI_MENU*, int);  /* callback function */
    struct JSUI_MENU *child;      /* to allow nested menus */
    int flags;                    /* flags about the menu state */
    void *dp;                     /* any data the menu might require */
} JSUI_MENU;

int menu_stub ( int msg, JSUI_MENU * menu, int c );

#define JSUI_MENU_END  {NULL}
#define JSUI_MENU_BAR  {"", menu_stub, NULL, 0, NULL},


#ifdef NEVER
typedef struct JSUI_MENU_RUNNING  /* information about a popup menu */
{
    JSUI_MENU *menu;               /* the menu itself */
    struct JSUI_MENU_RUNNING *parent; /* the parent menu, or NULL for root */
    int x, y;                      /* screen position of the menu */
    int (*proc) (void);            /* callback function */
} JSUI_MENU_INFO;
#endif



/* dialog control structure  --  this is an internal structure */
typedef struct JSUI_DIALOG_RUNNING
{  
    JSUI_DIALOG *dialog;	/* pointer to the associated dialog */
    int x,y;			/* screen position */
    int focus_obj;		/* which object has focus in the dlg */
    int flags;			/* flags -- dirty, hidden, etc */

    IPAGE * buffer;		/* video buffer to use */
    int vbx, vby;		/* offsets into the video buffer for xy */
    int mx, my, mb;		/* mouse position based on inner frame origin */

    struct JSUI_DIALOG_RUNNING * prev;
    struct JSUI_DIALOG_RUNNING * next;
} JSUI_DIALOG_RUNNING;


/************************************************************
 */
 
/* if the event filter doesn't find a window that can account
   for the mouse position, it will send the event down to the 
   fallback callback function, protyped here:
*/
typedef void (*filterEventsFallback)
(
    int event,		/* JSUI_EVENT_--- */
    int value1,		/* x, key */
    int value2,		/* y */
    int value3		/* mouse */
);

extern filterEventsFallback _fallback;

// do internal setup stuff
int jsui_setup(SDL_Surface * screen, filterEventsFallback fallback);

// clean up, when we're going to exit
void jsui_upset(void);

// standard event poll mechanism 
void jsui_poll(SDL_Surface * screen);


////////////////////////////////////////
// "run" a dialog, it gets focus.
int jsui_dialog_run(JSUI_DIALOG * dlg);

// "run" a dialog modally, it locks focus.
int jsui_dialog_run_modal(JSUI_DIALOG * dlg);

// "run" a menu
//int jsui_dialog_run_menu(JSUI_MENU * menu, int x, int y);

// "close" a dialog, it gets removed from the active list.
void jsui_dialog_close(JSUI_DIALOG * dlg);


////////////////////////////////////////
// tell one windows' of dialogs something
int jsui_dialog_send_message(JSUI_DIALOG *dlg, int msg, int c);

// tell one dialog element something
int jsui_dialog_tell_message(JSUI_DIALOG *basedlg, int which_one, int msg);

// tell one dialog element something specific
int jsui_dialog_tell_msginfo(JSUI_DIALOG *basedlg, int which_one, int msg, int c);

// tell everything in the active list a message
void jsui_dialog_broadcast(int msg, int value);


////////////////////////////////////////
// raise the dialog to the top of the screen
void jsui_dialog_raise(JSUI_DIALOG * dlg); 

// lower it down one level
void jsui_dialog_lower(JSUI_DIALOG * dlg);


////////////////////////////////////////
// reposition the dialog.  this is the absolute position
int jsui_dialog_position(JSUI_DIALOG * dlg, int x, int y);

// center the dialog in the screen
int jsui_dialog_center(JSUI_DIALOG * dlg);


////////////////////////////////////////
// figure out which element of the dialog has focus.  returns the index
JSUI_DIALOG * jsui_focus_find_active(int screen_x, int screen_y);

// figure out which element of the dialog has the mouse over it.  ret the index
int jsui_focus_find_widget(JSUI_DIALOG * dlg, int dialog_x, int dialog_y);

////////////////////////////////////////
// if the dialog is running, return 1, else 0
int jsui_is_running(JSUI_DIALOG * dlg);


/************************************************************
 * Some globals
 */

extern JSUI_DIALOG_RUNNING * jsui_active_list;  /* internal active list */

extern int jsui_request_quit;

extern int jsui_key_shift;
extern int jsui_key_control;

// colors
#define JSUI_C_ZERO	0	/* black */
#define JSUI_C_BG	26	/* background color */
#define JSUI_C_FG	0	/* foreground color */
#define JSUI_C_BRIGHT	1	/* bright 3D edge color */
#define JSUI_C_DARK	21	/* dark 3D edge color */
#ifdef NEVEr
#define JSUI_C_DISABLED	23	/* disabled items text and shading */
#endif
#define JSUI_C_DISABLED	0	/* disabled items text and shading */
#define JSUI_C_HILITE	8	/* hilit element color */
#define JSUI_C_LOLITE	0	/* compliment to the above color */
#define JSUI_C_DB	9	/* color for the dragbar 9 or 15*/
#define JSUI_C_DBIDLE   23	/* color for the dragbar */
#define JSUI_C_CB	10	/* color for the close button */
#define JSUI_C_CBIDLE   23	/* color for the close button */
#define JSUI_C_PROG     15	/* progress bar color */
#define JSUI_C_CURSOR   5	/* cursor for text entry widget */

#define JSUI_C_WFG	JSUI_C_FG    /* for user buttons -- foreground */
#define JSUI_C_WBG	JSUI_C_BG    /* for user buttons -- background */


/************************************************************
 * utility functions
 */

/* jsui_strlen  - returns the length in pixels of the text string
 */
int jsui_strlen(char * text);



/************************************************************
 * built-in widgets 
 */

/* jsui_widget_frame
 * this _NEEDS_ to be the first widget in the dialog 
 *
 * x,y  origin of the frame
 * w,h  size of the usable frame inside of it
 * d1   flags for the frame configuration
 * dp   titlebar text
 */
int jsui_widget_frame(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* frame uses these flags in d1 */
#define JSUI_FRAME_DRAGGABLE    0x0001  /* the frame is draggable */
#define JSUI_FRAME_RESIZE       0x0002  /* the frame is resizable... sheah. right. */
#define JSUI_FRAME_CLOSE        0x0004  /* close button is shown */
#define JSUI_FRAME_TITLEBAR     0x0008  /* title bar is shown */
#define JSUI_FRAME_GRABFOCUS    0x0010  /* the frame grabs focus -> modal */

#define JSUI_FRAME_CENTER       0x0100  /* frame is centered */
#define JSUI_FRAME_EAST         0x0200  /* frame is on the right side */
#define JSUI_FRAME_WEST         0x0400  /* frame is on the left side */
#define JSUI_FRAME_NORTH        0x0800  /* frame is on the top side */
#define JSUI_FRAME_SOUTH        0x1000  /* frame is on the bottom side */


/* jsui_widget_bitmap
 *
 * x,y  placement
 * w,h  clip width/height  (or scale?)
 * dp   pointer to an IPAGE
 * d1   doublesize the image?
 * d2   lowered box instead of raised box?
 */
int jsui_widget_image(int msg, void * vjdr, JSUI_DIALOG * d, int c);


/* jsui_widget_text
 *
 * x,y  placement of the upper left of the text
 * dp   pointer to the text string
 * d1   flags for justification
 */
int jsui_widget_text(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* justification - set d1 with these */
#define JSUI_T_LEFT          0
#define JSUI_T_RIGHT         1
#define JSUI_T_CENTER        2
#define JSUI_T_JUSTIFIED     3
#define JSUI_T_ANCIENT       3


/* jsui_widget_3Dbox 
 *
 * dp   pointer to text for the name
 * d1   flags for text justification
 * d2   Raised or lowered flag
 */
int jsui_widget_3Dbox(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* 3d box flags */
#define JSUI_3_RAISED        0
#define JSUI_3_LOWERED       1


/* jsui_widget_button 
 *
 * dp   pointer to text for "off"
 * dp2  pointer to text for "on"
 * dp3  pointer to callback function.
 * d1   button flags
 * d2   radio group
 */
int jsui_widget_button(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* button flags */
#define JSUI_B_ON_R          0x0001
#define JSUI_B_ON_L          0x0002
#define JSUI_B_ON	     (JSUI_B_ON_R | JSUI_B_ON_L)
#define JSUI_B_STICKY        0x0004
#define JSUI_B_RADIO         0x0008
#define JSUI_B_CHECKBOX      0x0010
#define JSUI_B_FORCEON_R     0x0020 /* for dragging.. force it on*/
#define JSUI_B_FORCEON_L     0x0030 /* for dragging.. force it on*/

/* callback is of the form:
       int function(JSUI_DIALOG *d, int object id)
 */


/*
 *  jsui_widget_imagebutton
 * dp  	pointer to an array of IPAGE_XREF structures (idle)
 *  	   dp[0] = idle
 *	   dp[1] = left click
 *	   dp[2] = right click
 *	   (set dp to null for no image (which would be stupid))
 *	   (set elements to NULL for no image)
 * d1 	same as above
 * d2 	same as above
 */
int jsui_widget_imagebutton(int msg, void * vjdr, JSUI_DIALOG * d, int c);

typedef struct _ipage_xref {
    IPAGE * img_buffer;
    int x; 	// x offset to start of structure
    int y; 	// y offset to start of structure
    int w; 	// width of display area
    int h; 	// height of display area
    int pd;	// post-double the pixels. (after range clip, double the size)
} IPAGE_XREF;


/* jsui_widget_progress  - progress bar
 *
 * d1  max
 * d2  current
 */
int jsui_widget_progress(int msg, void * vjdr, JSUI_DIALOG * d, int c);
 

/* jsui_widget_keyboard
 *
 * - keyboard shortcut
 */
int jsui_widget_keyboard(int msg, void * vjdr, JSUI_DIALOG * d, int c);


/* jsui_widget_edit  - editable text box
 * 
 * dp  points to a char buffer
 * d1  max characters
 * d2  cursor position
 */
int jsui_widget_edit(int msg, void * vjdr, JSUI_DIALOG * d, int c);


/* jsui_widget_list
 * d1  selected item
 * d2  scroll position
 * dp  pointer to a null terminated list of strings (argv)
 * dp3 pointer to a callback function (below)
 */ 
int jsui_widget_list(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* callback is of the form:
	int list_cb(
		JSUI_DIALOG * dlg,
		int focusitem,
		int message,
		int indexselected,
		char * selectedtext
	);
*/



/* jsui_widget_textbox  - scrolling text display
 *
 * dp  text buffer
 * d1  number of lines (internal)
 * d2  scroll position
 */
int jsui_widget_textbox(int msg, void * vjdr, JSUI_DIALOG * d, int c);


/* jsui_widget_slider
 *
 * h>w  vertical slider, otherwise horizontal slider
 * d1  max value  [0.. d1]
 * d2  current value
 * dp  pointer to a callback - for when it changes
 */
int jsui_widget_slider(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* callback is of the form:
       int function(*dp3, int d2)
 */


/* jsui_widget_menu
 *
 * dp  pointer to the menu to use
 */
int jsui_widget_menu(int msg, void * vjdr, JSUI_DIALOG * d, int c);


/* jsui_widget_colordisplay
 *
 * d->d1 fg color
 * d->d2 bg color
 * d->dp  callback function of the form:
		int function(int msg, JSUI_DIALOG * d)
 */
int jsui_widget_colordisplay(int msg, void * vjdr, JSUI_DIALOG * d, int c);

/* jsui_widget_colorpicker
 *
 * d->d1 number of colors
 * d->dp  callback function of the form: 
		int function(int msg, JSUI_DIALOG * d, int colornumber)
 */
int jsui_widget_colorpicker(int msg, void * vjdr, JSUI_DIALOG * d, int c);



/* jsui_widget_timetick
 *
 *     just receives time tick events when they happen.
 */
int jsui_widget_timetick(int msg, void * vjdr, JSUI_DIALOG * d, int c);




/************************************************************
 * built-in dialogs
 */

/* jsui_bid_file_select  - file selector 
 *   message   caption
 *   path      base path to start the selector in, result is returned in this
 *   max       max length of path.  (buffersize)  make the path buffer big.
 *   ext       list of extensions to show, semicolon seperated  
 */



int
do_filereq(
        SDL_Surface * screen,
        char * title,
        char * button,
        char * dir_buffer,
        char * file_buffer,
        int buffersize
);

int do_acceptor(
        SDL_Surface * screen,
        char * title,
        char * line1,
        char * line2
);

int
do_fontreq(
        SDL_Surface * screen,
        char * buffer,
        int buffersize
);



/* jsui_bid_alert  - popup alert window
 * s1 s2 s3   message text.  will be centered.  three lines.
 * b1 b2 b3   button text for left, center, and right.  set NULL to obliterate
 * c1 c2 c3   return values for each of the three buttons.
 */
int jsui_bid_alert(char * s1, char * s2, char * s3, 
		   char * b1, char * b2, char * b3, 
                   int    c1, int    c2, int    c3);

#endif



/****************************************************************************
 ****************************************************************************
 **************      misc
 */
 
int extent_low_3(int, int, int);
int extent_high_3(int, int, int);

#define extent_low_2(A,B)	extent_low_3( (A) , (B) , -1 )
#define extent_high_2(A,B)	extent_low_3( (A) , (B) , -1 )

#ifndef MAX
#define MAX(A,B)   ( ((A)>(B)) ? (A) : (B) )
#endif

#ifndef MIN
#define MIN(A,B)   ( ((A)>(B)) ? (B) : (A) )
#endif 

#ifndef MAX3
#define MAX3(A,B,C)	 MAX( (A) , ( MAX( (B) , (C) ) ) )
#endif

#ifndef MIN3
#define MIN3(A,B,C)	 MIN( (A) , ( MIN( (B) , (C) ) ) )
#endif

#ifndef ABS
#define ABS(A)     ( ((A)>0) ?(A) : (-(A)) )
#endif

#ifndef MID
#define MID(A,B,C) ( MAX (A), MIN((B),(C)) )
#endif

#ifndef SGN
#define SGN(A)     ( ((A)>=0) ? 1 : -1)
#endif

