/* RABLe-Paint, by Aaron Roberts and Bob Lanham, 1999 */

#include <stdlib.h>  /* exit() function defined here */
#include <stdio.h>   /* puts(), sprintf(), printf(), etc. */
#include <string.h>  /* strcpy() */
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfxmacros.h>

#include <aros/oldprograms.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/gadgetclass.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>

#include "rable_draw.h"
#include "rable_gadgets.h"
#include "rable.h"
#include "rable_fills.h"
#include "iffp/ilbm.h"

#ifdef LATTICE
int CXBRK(void)    { return(0); }  /* Disable SAS CTRL/C handling */
int chkabort(void) { return(0); }  /* really */
#endif

/* globals */
struct IntuitionBase *IntuitionBase;
struct Library *GadToolsBase;
struct Library *AslBase;
struct Library *DataTypesBase;
struct Library *MathTransBase;
struct Library *IFFParseBase;
struct GfxBase *GfxBase;
struct Screen *screen1;
struct Window *window1; /* paint */
struct Window *window2; /* tools */
struct Window *window3; /* palette*/
struct Menu *strip;
struct FileRequester *fr;
struct ScreenModeRequester *screenmode;
struct Gadget *last_gadget = NULL;  /* address of last selected tool  */
struct Gadget *last_pen = NULL;     /* address of last selected color */
struct Gadget *last_fill = NULL;    /* address of last chosen fill */

int curr_pen=2;

/* used for fill operations in the drawing routines */
struct AreaInfo ainfo;
struct TmpRas temprast;
PLANEPTR tbuffer;

#define AREA_BYTES 100
WORD buffer[AREA_BYTES / 2];
void *ras;
WORD wMaxX, wMaxY;

/* initial color table, 32 color palette */
UWORD colortable[] =
{
  0x777,0x000,0xfff,0x00f,0x0f0,0xff0,0xf00,0x0ff,
  0x0b6,0x0dd,0x0af,0x07c,0xf0f,0x70f,0xc0e,0xc08,
  0x620,0xef2,0xa52,0xfca,0x333,0xeee,0x555,0x666,
  0x777,0x888,0x999,0xaaa,0xccc,0xddd,0x444,0xfff
};

#ifdef MAXON
#define __chip
#endif

/* sprite data for pointer */
UWORD /*__chip*/ pointerdat[] = {

   0x0000, 0x0000,
   
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0500, 0x0000,
   0xf8f8, 0x0000,
   0x0500, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,
   0x0200, 0x0000,

   0x0000, 0x0000
};

extern struct Gadget cgad1; /* for default values */
extern struct Gadget pgad2;

extern struct Gadget reqgad4;
extern struct IntuiText itext5;
extern struct Border border2;
extern struct StringInfo si1;
extern struct StringInfo si2;
extern struct Gadget rslider;
extern struct Gadget gslider;
extern struct Gadget bslider;
extern struct PropInfo slider_info1;
extern struct PropInfo slider_info2;
extern struct PropInfo slider_info3;

/************************************************************/

main(int argc, char *argv[])
{
    InitLibs();
    screenmode=openASL();
    if(screenmode==NULL) clean_exit(NULL, RETURN_FAIL );
    setup_display();
    if(!setup_menu())
        clean_exit("Cannot open menu!", RETURN_FAIL );
    process_events();
    clean_exit(NULL, RETURN_OK);
}

void wbmain(void)   /* If started from WB */
{
    long temp = 0;
    char tempchar = NULL;
    main(0, NULL);
}

/*************************************************************/

void InitLibs(void)
{
    IntuitionBase = (struct IntuitionBase *)OpenLibrary
                    ( "intuition.library",39 );
    if (IntuitionBase == NULL)
        clean_exit("Cannot open intuition.library v39!",RETURN_FAIL);

    GadToolsBase = OpenLibrary( "gadtools.library",0 );
    if (GadToolsBase == NULL)
        clean_exit("Cannot open gadtools.library!",RETURN_FAIL);

    AslBase = OpenLibrary( "asl.library",0 );
    if (AslBase == NULL)
        clean_exit("Cannot open asl.library!",RETURN_FAIL);

    DataTypesBase = OpenLibrary( "datatypes.library", 39 );
    if (DataTypesBase == NULL)
        clean_exit("Can not open datatypes.library", RETURN_FAIL);

    GfxBase = (struct GfxBase *)OpenLibrary( "graphics.library",0 );
    if (GfxBase == NULL)
        clean_exit("Cannot open Gfx.library!",RETURN_FAIL);

    IFFParseBase = OpenLibrary( "iffparse.library",0 );
    if (IFFParseBase ==NULL)
        clean_exit("Cannot open iffparse.library!",RETURN_FAIL);
}

/*************************************************************/

struct ScreenModeRequester *openASL(void)
{
struct ScreenModeRequester *smr;
BOOL result=FALSE;

struct TagItem smrtags[] =
    {
    ASLSM_TitleText, (ULONG)"Select screen for RABLe Paint",
    ASLSM_InitialLeftEdge, 40,
    ASLSM_InitialTopEdge, 20,
    ASLSM_InitialWidth, 340,
    ASLSM_InitialHeight, 180,
    ASLSM_DoWidth, TRUE,
    ASLSM_DoHeight, TRUE,
    ASLSM_DoDepth, FALSE,
    ASLSM_DoOverscanType, FALSE,
    ASLSM_DoAutoScroll, TRUE,
    ASLSM_MinWidth, 640,
    ASLSM_MinHeight, 400,
    TAG_DONE
    };

if(smr=AllocAslRequest(ASL_ScreenModeRequest,NULL))
    {
    result=AslRequest(smr,smrtags);
    FreeAslRequest(smr);
    }
if(result) return(smr);
else return(FALSE);
}

/*************************************************************/

void setup_display(void)
{
USHORT tool_left,pal_top,temp_w,temp_h;
#define WIDE (screenmode->sm_DisplayWidth)
#define HIGH (screenmode->sm_DisplayHeight)
tool_left = WIDE-130;
pal_top = HIGH-100;

    /* open the main screen */
    screen1 = OpenScreenTags(NULL,
              SA_Title,     (ULONG)"Welcome to RABLe-Paint",
              SA_Depth,     5,         
              SA_DisplayID, screenmode->sm_DisplayID,
              TAG_DONE);

              if (screen1 == NULL)
              clean_exit("Cannot open screen!",RETURN_FAIL);

    /* initialize colortable, now so all windows
    have proper background color */
    LoadRGB4(&screen1->ViewPort,colortable,32);

    /* open the paint window */
    window1 = OpenWindowTags(NULL,
              WA_Left,         0,   /* offsets */
              WA_Top,          12,
              WA_Width,        320,
              WA_Height,       200,
              WA_MinWidth,     150, 
              WA_MinHeight,    100,
              WA_MaxWidth,     ~0,
              WA_MaxHeight,    ~0,
              WA_CloseGadget,  FALSE, /* gadgets */
              WA_DepthGadget,  TRUE,
              WA_DragBar,      TRUE,
              WA_SizeGadget,   FALSE,
              WA_Activate,     TRUE, /* attributes */
              WA_ReportMouse,  TRUE,
              WA_IDCMP,        IDCMP_MENUPICK |
                               IDCMP_VANILLAKEY |
                               IDCMP_CHANGEWINDOW | /* need for resizing */
                               IDCMP_MOUSEBUTTONS |
                               IDCMP_GADGETUP|
                               IDCMP_MOUSEMOVE,
              WA_CustomScreen, screen1,/* Attach window to screen */
              WA_Title,        "Canvas",
              TAG_DONE);

              if (window1 == NULL)
              clean_exit("Cannot open window!",RETURN_FAIL); 

    /* resize paint window to standard size: 320x200 */
    ChangeWindowBox(window1,0,12,320+window1->BorderLeft
            +window1->BorderRight,200+window1->BorderTop
            +window1->BorderBottom);

    /* Change pointer for drawing */
    SetPointer(window1, pointerdat, 14, 14, -7, -7);
	
    /* Setup raster and buffer for Area functions */
    wMaxX = 640; /* Maxused temporarily*/
    wMaxY = 320; /* same */
	
    ras = AllocRaster(wMaxX, wMaxY);
    if (!ras)
	clean_exit("Unable to setup temp memory!", RETURN_FAIL);
    InitTmpRas(&temprast, ras, RASSIZE(wMaxX, wMaxY));
    InitArea(&ainfo, buffer, AREA_BYTES / 5);
    window1->RPort->TmpRas = &temprast;
    window1->RPort->AreaInfo = &ainfo;
	
    /* open the tools window */
    temp_w=window1->BorderLeft+window1->BorderRight+40;
    temp_h=window1->BorderTop+window1->BorderBottom+120;
    window2 = OpenWindowTags(NULL,
              WA_Left,         tool_left,
              WA_Top,          30,
              WA_Width,        temp_w,
              WA_Height,       temp_h,
              WA_GimmeZeroZero,TRUE,
              WA_DepthGadget,  TRUE,
              WA_DragBar,      TRUE,
              WA_IDCMP,        IDCMP_MENUPICK |
                               IDCMP_VANILLAKEY |
                               IDCMP_GADGETDOWN,
              WA_Gadgets,      get_gadgets(),
              WA_CustomScreen, screen1,
              TAG_DONE);

              if (window2 == NULL)
              clean_exit("Cannot open window!",RETURN_FAIL); 

    /* open the palette window */
    temp_w=window1->BorderLeft+window1->BorderRight+256;
    temp_h=window1->BorderTop+window1->BorderBottom+50;
    window3 = OpenWindowTags(NULL,
              WA_Left,         20,
              WA_Top,          pal_top,
              WA_Width,        temp_w,
              WA_Height,       temp_h,
              WA_GimmeZeroZero,TRUE,
              WA_DepthGadget,  TRUE,
              WA_DragBar,      TRUE,
              WA_IDCMP,        IDCMP_MENUPICK |
                               IDCMP_VANILLAKEY |
                               IDCMP_GADGETUP|
                               IDCMP_GADGETDOWN,
              WA_Gadgets,      get_pal(),
              WA_CustomScreen, screen1,
              TAG_DONE);

              if (window3 == NULL)
              clean_exit("Cannot open window!",RETURN_FAIL); 

    /* set up file requester */
    fr = (struct FileRequester *)
        AllocAslRequestTags(ASL_FileRequest,
              ASL_Window,     window1,
              ASL_Hail,       (ULONG)"Load or Save Picture",
              ASL_Height,     180,
              ASL_Width,      300,
              ASL_LeftEdge,   20,
              ASL_TopEdge,    20,
              ASL_OKText,     (ULONG)"Okay",
              ASL_CancelText, (ULONG)"Cancel",
              TAG_DONE);

              if (fr == NULL)
              clean_exit("Cannot open file requester!",RETURN_FAIL); 
}

/*************************************************************/

BOOL setup_menu(void)
{
    APTR *vinfo;

    struct NewMenu mynewmenu[] = {
           { NM_TITLE, "Project",       0 , 0, 0, 0},
           {  NM_ITEM, "New",          "N", 0, 0, 0},
           {  NM_ITEM, "Open Picture", "O", 0, 0, 0},
           {  NM_ITEM, "Save IFF",     "S", 0, 0, 0},
           {  NM_ITEM, NM_BARLABEL,     0 , 0, 0, 0},
           {  NM_ITEM, "About",         0 , 0, 0, 0},
           {  NM_ITEM, "Quit",         "Q", 0, 0, 0},
           { NM_TITLE, "Settings",      0 , 0, 0, 0},
           {  NM_ITEM, "Canvas Size",  "C", 0, 0, 0},
           {   NM_END, NULL,            0 , 0, 0, 0},
    };

    strip = CreateMenus(mynewmenu, TAG_END);
    vinfo = GetVisualInfo(screen1, TAG_END);
    LayoutMenus(strip, vinfo, TAG_END);
    FreeVisualInfo(vinfo);

    if(SetMenuStrip(window1, strip))
        {
        if(SetMenuStrip(window2, strip))
            {
            if(SetMenuStrip(window3, strip))
            return TRUE;
            }
        }
    return FALSE;
}
/*************************************************************/

void process_events(void) /* find out which window had event */
{
    ULONG signals;
    BOOL done=FALSE;

    /* initialize "last" tool and color */
    last_gadget = &cgad1;
    last_pen = &pgad2;

    while(!done)
        {
        signals = Wait( (1L << window1->UserPort->mp_SigBit) |
                        (1L << window2->UserPort->mp_SigBit) |
                        (1L << window3->UserPort->mp_SigBit));
        if (signals & (1L << window1->UserPort->mp_SigBit))
            done = handleIDCMP(window1);
        else if (signals & (1L << window2->UserPort->mp_SigBit))
            done = handleIDCMP(window2);
        else if (signals & (1L << window3->UserPort->mp_SigBit))
            done = handleIDCMP(window3);
        }
}
/*******************************************************************/

BOOL handleIDCMP(struct Window *win)
{
struct IntuiMessage *message;
ULONG mclass;
USHORT code;
BOOL done = FALSE;
SHORT mousex, mousey;

struct Gadget *gadget;

while (NULL != (message = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
    code   = message->Code;
    mousex = message->MouseX;
    mousey = message->MouseY;
    mclass = message->Class;

    if ((mclass==IDCMP_GADGETDOWN)||(mclass==IDCMP_GADGETUP))
        gadget=(struct Gadget *)message->IAddress;

    ReplyMsg((struct Message *)message);

    switch (mclass)
        {
        case IDCMP_MENUPICK:
            done = menu_handler(code, done);
            break;

        case IDCMP_VANILLAKEY:    /* for future key codes */
            break;

        case IDCMP_MOUSEBUTTONS:
        case IDCMP_MOUSEMOVE:
            decode_gadget(last_gadget, message); /* must send last selected tool */
            break;

        case IDCMP_GADGETDOWN:
            if(gadget->GadgetID < 50) /* it's a toolbox gadget */
                tool_handler(gadget);
					
            else if((gadget->GadgetID > 49) && (gadget->GadgetID < 100)) 
                fill_handler(gadget);   /* fill gadget */             	

            else if((gadget->GadgetID > 100)&&(gadget->GadgetID < 133))
                palette_handler(gadget); /*palette*/
	    break;

        case IDCMP_GADGETUP:
            if(gadget->GadgetID==503) resize_handler();  /* canvas size */
            else if((gadget->GadgetID>600) && (gadget->GadgetID<604))
                slider_handler();       /* color slider */
            break;
        }
    }
return(done);
}

/****************************************************************/

void palette_handler(struct Gadget *gadget)
{
last_pen->Flags=last_pen->Flags ^ GFLG_SELECTED;
RefreshGList(get_pal(),window3, NULL, 35);
last_pen = gadget;
/* set paintbrush color */
curr_pen = (gadget->GadgetID)-100;
slider_refresh();
}
/****************************************************************/

void resize_handler (void)
{
   int w, h;
   w=si1.LongInt;
   if(w<150) w=150; /* Minimum size is 150x100 to make sure */
   h=si2.LongInt;   /* size requester doesn't get clipped.  */
   if(h<100) h=100;

   /* resize window */
   ChangeWindowBox(window1,0,12,w+window1->BorderLeft
            +window1->BorderRight,h+window1->BorderTop
            +window1->BorderBottom);
}
/****************************************************************/
/* When a slider is moved, the ViewPort's ColorMap will change. */

void slider_handler (void)
{
   #define NOTCH (MAXPOT/16)
   int r,g,b;

   r=(slider_info1.HorizPot)/NOTCH;
   if(r==16) r=15;
   g=(slider_info2.HorizPot)/NOTCH;
   if(g==16) g=15;
   b=(slider_info3.HorizPot)/NOTCH;
   if(b==16) b=15;

   SetRGB4(&screen1->ViewPort,curr_pen-1,r,g,b);
}
/****************************************************************/

void slider_refresh (void)
{
/* When a palette color is clicked on, sliders need updating. */
/* This is also done when "new" or "load" is selected. */

   UWORD RGBvalue;
   struct ColorRegister cmapReg;

   RGBvalue=GetRGB4(screen1->ViewPort.ColorMap,curr_pen-1);
   cmapReg.red    = (RGBvalue>>4) & 0xf0;
   cmapReg.red   |= (cmapReg.red >> 4);
   cmapReg.green  = (RGBvalue) & 0xf0;
   cmapReg.green |= (cmapReg.green >> 4);
   cmapReg.blue   = (RGBvalue<<4) & 0xf0;
   cmapReg.blue  |= (cmapReg.blue >> 4);

   NewModifyProp(&rslider,window3,NULL,FREEHORIZ|AUTOKNOB,
                 MAXPOT/255*cmapReg.red,0,MAXBODY/16,0,1);

   NewModifyProp(&gslider,window3,NULL,FREEHORIZ|AUTOKNOB,
                 MAXPOT/255*cmapReg.green,0,MAXBODY/16,0,1);

   NewModifyProp(&bslider,window3,NULL,FREEHORIZ|AUTOKNOB,
                 MAXPOT/255*cmapReg.blue,0,MAXBODY/16,0,1);
}
/****************************************************************/

void fill_handler(struct Gadget *gadget)
{
struct RastPort *rp = window1->RPort;

switch (gadget->GadgetID)
{
	case fill_1:
				SetAfPt(rp, fill0_pat, 3);
				break;
				
	case fill_2:
				SetAfPt(rp, fill1_pat, 3);
				break;
				
	case fill_3:
				SetAfPt(rp, fill2_pat, 3);
				break;
				
	case fill_4:
				SetAfPt(rp, fill3_pat, 4); /* This fill uses a 16 x 16 pattern */
				break;
				
}

/* If you want to unselect a fill pattern */
if(last_fill != NULL)
	last_fill->Flags = last_fill->Flags ^ GFLG_SELECTED;
if(last_fill == gadget)
{
	last_fill->Flags = last_fill->Flags ^ GFLG_SELECTED;
	SetAfPt(rp, NULL, 0);	/* Reset fill to solid */
	gadget = NULL;
}
RefreshGList(get_gadgets(), window2, NULL, total_gadgets);
/* store current tool to unselect later */
last_fill = gadget;
}

/****************************************************************/

void tool_handler(struct Gadget *gadget)
{
if(last_gadget != NULL)  /* unselect last tool */
	 last_gadget->Flags = last_gadget->Flags ^ GFLG_SELECTED;
 /* redraw the toolbox */
RefreshGList(get_gadgets(), window2, NULL, total_gadgets);
/* store current tool to unselect later */
last_gadget = gadget;
}
                
/****************************************************************/

BOOL menu_handler(UWORD menuNumber, BOOL done)
{
UWORD menuNum;
UWORD itemNum;
UWORD subNum;
UBYTE filepath[512];
struct MenuItem *item;

while ((menuNumber != MENUNULL) && (!done))
{
	item = ItemAddress(strip, menuNumber);
	menuNum = MENUNUM(menuNumber);
	itemNum = ITEMNUM(menuNumber);
	subNum = SUBNUM(menuNumber);
                    
	if ((menuNum==0) && (itemNum==0)) /* user chose new */
	{
		SetAPen(window1->RPort, 0);
		LoadRGB4(&screen1->ViewPort,colortable,32);
                slider_refresh();

                ChangeWindowBox(window1,0,12,
                   320+window1->BorderLeft
                   +window1->BorderRight,200+window1->BorderTop
                   +window1->BorderBottom);

		Delay(20);
		RectFill(window1->RPort,window1->BorderLeft,
			window1->BorderTop,
			window1->Width-window1->BorderRight,
			window1->Height-window1->BorderBottom);
	}
	else if ((menuNum==0) && (itemNum==5)) /* user quit */
		done = TRUE;
	else if ((menuNum==0) && (itemNum==1)) /* user open file */
	{
		if (AslRequest(fr, NULL))
		{
                        strcpy(filepath,fr->fr_Drawer);
			AddPart(filepath,fr->fr_File,sizeof(filepath));
			LoadPic(filepath);
                        slider_refresh();
		}
	}
	else if ((menuNum==0) && (itemNum==4)) /* user about */
		about_window();

        else if ((menuNum==0) && (itemNum==2)) /* user save file */ 
	{
		if (AslRequest(fr, NULL))
		{
                        strcpy(filepath,fr->fr_Drawer);
			AddPart(filepath,fr->fr_File,sizeof(filepath));
			SavePic(filepath);
		}
	}


	else if ((menuNum==1) && (itemNum==0)) /* canvas size */
	{
                canvas_size();
        }


	menuNumber = item->NextSelect;
}
return(done);
}

/****************************************************************/

void decode_gadget(struct Gadget *picked, struct IntuiMessage *message)
{

    switch (picked->GadgetID)
    {
	case square_n:
		draw_square(window1, normal, message);
		break;

	case square_f:
		draw_square(window1, filled, message);
		break;

	case circle_n:
		draw_circle(window1, normal, message);
		break;

	case circle_f:
		draw_circle(window1, filled, message);
		break;

	case line:
		draw_line(window1, message);
		break;
		
	case oval_n:
		draw_oval(window1, normal, message);
		break;
		
	case oval_f:
		draw_oval(window1, filled, message);
		break;
		
	case freestyle:
		draw_free(window1, message);
		break;
		
	case rectangle_n:
		draw_rectangle(window1, normal, message);
		break;
		
	case rectangle_f:
		draw_rectangle(window1, filled, message);
		break;
		
	case triangle_n:
		draw_triangle(window1, normal, message);
		break;
		
	case triangle_f:
		draw_triangle(window1, filled, message);
		break;
    }
	
}

/****************************************************************/

void about_window(void)
{

struct EasyStruct myrequestor =
    {
    sizeof(struct EasyStruct),
    0,
    "About RABLe-Paint",
    "RABLe-Paint Version 0.9\nOct. 4, 1999\nby Aaron Roberts and Bob Lanham",
    "Okay"
    };
EasyRequest(window1, &myrequestor, NULL);
}

/****************************************************************/

void LoadPic(char *filename)
{
    Object *mypic=NULL;
    ULONG c,w,h,r,g,b,i;
    LONG *cregs;
    struct IntuiMessage *mess;
    BOOL done=FALSE;

    struct EasyStruct warning1 =
        {
        sizeof(struct EasyStruct),
        0,
        "ERROR LOADING PICTURE",
        "Unknown Picture File Format",
        "Okay"
        };

    struct EasyStruct warning2 =
        {
        sizeof(struct EasyStruct),
        0,
        "ERROR LOADING PICTURE",
        "32 Colors Maximum\n640x400 Pixels Maximum",
        "Okay"
        };

if(mypic = NewDTObject(filename,
    DTA_SourceType,  DTST_FILE,    /* source is a file */
    DTA_GroupID,     GID_PICTURE,  /* must be a picture */
    PDTA_Remap,      FALSE,
    TAG_DONE))
    {

    /* get attributes we are interested in */
    GetDTAttrs(mypic,
        PDTA_CRegs,           &cregs,
        PDTA_NumColors,       &c,
        DTA_NominalHoriz,     &w,
        DTA_NominalVert,      &h,
        TAG_DONE);

    if( w<=640 && h<=400 && c<33 && c>0 ) /* check max size and depth */
    {
        /* reset original colortable */
        LoadRGB4(&screen1->ViewPort,colortable,32);
        /* size window to fit picture */
        ChangeWindowBox(window1,0,12,w+window1->BorderLeft
            +window1->BorderRight,h+window1->BorderTop
            +window1->BorderBottom);

        while(!done) /* wait for window to resize */
            {
            WaitPort( window1->UserPort );
            while (mess=(struct IntuiMessage *)GetMsg(window1->UserPort))
                {
                if(mess->Class==IDCMP_CHANGEWINDOW) done=TRUE;
                ReplyMsg((struct Message *)mess);
                }
            }

        /* erase the image */
	SetAPen(window1->RPort, 0);
	RectFill(window1->RPort,window1->BorderLeft,
				window1->BorderTop,
				window1->Width-window1->BorderRight,
				window1->Height-window1->BorderBottom);
	
        /* load new colortable */
        for(i=0;i<c;i++)
        {
            r=cregs[i*3+0];
            g=cregs[i*3+1];
            b=cregs[i*3+2];
            SetRGB32(&screen1->ViewPort,i,r,g,b);
        }

        /* set the attributes of our datatype object*/
        SetDTAttrs(mypic,NULL,NULL,
            GA_Left,    window1->BorderLeft,
            GA_Top,     window1->BorderTop,
            GA_Width,   w,
            GA_Height,  h,
            TAG_DONE);

        AddDTObject(window1,NULL,mypic,-1);
        Delay(20);
        RefreshDTObjects(mypic,window1,NULL,NULL);
        RemoveDTObject(window1,mypic);
    }
  else EasyRequest(window1, &warning2, NULL); /* warn: picture too big */

  DisposeDTObject(mypic);
  }
else EasyRequest(window1, &warning1, NULL); /* warn: wrong type of file */
 }

/****************************************************************/

void SavePic(char *filename)
{
   struct IFFHandle *iff;
   int depth =screen1->ViewPort.RasInfo->BitMap->Depth;
   int width =window1->Width-window1->BorderLeft-window1->BorderRight;
   int height=window1->Height-window1->BorderTop-window1->BorderBottom;
   int ncolors=screen1->ViewPort.ColorMap->Count; /* number of colors */
   struct BitMapHeader bmhd;
   int iPlane, iRow, i;
   BYTE *planes[MAXSAVEDEPTH+1]; /* array of pointers to planes */
   struct BitMap *bmap;
   UWORD RGBvalue;
   struct ColorRegister cmapReg;
   ULONG modeid;
   struct DisplayInfo DI;
   LONG error;

   modeid=GetVPModeID(&screen1->ViewPort);
   GetDisplayInfoData(NULL, (UBYTE *)&DI, sizeof(struct DisplayInfo),
                      DTAG_DISP, modeid);

   WindowToFront(window1); /* in case another window overlaps it */
   Delay(20);

   /* init BitMapHeader */
   bmhd.bmh_Width	= width;
   bmhd.bmh_Height	= height;
   bmhd.bmh_Left	= 0;
   bmhd.bmh_Top		= 0;
   bmhd.bmh_Depth	= depth;
   bmhd.bmh_Masking	= 0;
   bmhd.bmh_Compression	= 0;  /* we're not using compression */
#warning Deactivated the following line
//   bmhd.flags		= BMHDF_CMAPOK;
   bmhd.bmh_Transparent	= 0;
   bmhd.bmh_XAspect	= DI.Resolution.x;
   bmhd.bmh_YAspect	= DI.Resolution.y;
   bmhd.bmh_PageWidth	= width;
   bmhd.bmh_PageHeight	= height;

   /* init IFFHandle */
   if(!(iff=AllocIFF())) {printf("AllocIFF failed.\n"); return;}

   if(!(iff->iff_Stream=Open(filename, MODE_NEWFILE)))
             {printf("Cannot open iff_Stream.\n");
              FreeIFF(iff);
              return;}

   InitIFFasDOS(iff);
   error=OpenIFF(iff,IFFF_WRITE);
   if(error) {printf("OpenIFF error = %ld\n",error);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;}

   /* create FORM chunk */
   error=PushChunk(iff,ID_ILBM,ID_FORM,IFFSIZE_UNKNOWN);
   if(error) {printf("PushChunk FORM error = %ld\n",error);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;}

   /* write BMHD chunk */
   error=PushChunk(iff,ID_ILBM,ID_BMHD,sizeof(struct BitMapHeader));
   if(error) {printf("PushChunk BMHD error = %ld\n",error);
              PopChunk(iff);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;}

   if(WriteChunkBytes(iff,&bmhd,sizeof(bmhd))!=sizeof(bmhd))
              {    
              printf("WriteChunkBytes BMHD error.\n");
              PopChunk(iff);
              PopChunk(iff);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;
              }

   PopChunk(iff);
   
   /* write CMAP chunk */
   error=PushChunk(iff,ID_ILBM,ID_CMAP,ncolors*3);
   if(error) {printf("PushChunk CMAP error = %ld\n",error);
              PopChunk(iff);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;}

   for(i=0;i<ncolors;i++)
      {
      RGBvalue=GetRGB4(screen1->ViewPort.ColorMap,i);
      cmapReg.red    = (RGBvalue>>4) & 0xf0;
      cmapReg.red   |= (cmapReg.red >> 4);
      cmapReg.green  = (RGBvalue) & 0xf0;
      cmapReg.green |= (cmapReg.green >> 4);
      cmapReg.blue   = (RGBvalue<<4) & 0xf0;
      cmapReg.blue  |= (cmapReg.blue >> 4);
      /* 3=size of one ColorRegister */
      if(WriteChunkBytes(iff,&cmapReg,3)!=3)
              {    
              printf("WriteChunkBytes CMAP error.\n");
              PopChunk(iff);
              PopChunk(iff);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;
              }
      }

   PopChunk(iff);

   /* initialize a temporary BitMap */
   if(bmap=AllocVec(sizeof(struct BitMap),MEMF_CLEAR))
      {
      InitBitMap(bmap,depth,width,height);
      for(i=0;i<depth;i++)
         {
         bmap->Planes[i]=AllocRaster(width,height);
         if(bmap->Planes[i]==NULL)
           { printf("Could not allocate memory for raster.\n");
             for(i=0;i<depth;i++)
                 if(bmap->Planes[i])
                 FreeRaster(bmap->Planes[i],width,height);
             FreeVec(bmap);
             PopChunk(iff);
             CloseIFF(iff);
             Close(iff->iff_Stream);
             FreeIFF(iff);
             return;
           }
         BltClear(bmap->Planes[i],RASSIZE(width,height),0);
         }
      }
   else
      {  printf("Could not allocate memory for BitMap.\n");    
         PopChunk(iff);
         CloseIFF(iff);
         Close(iff->iff_Stream);
         FreeIFF(iff);
         return; 
      }

   /* copy area within window to the temporary BitMap */
   BltBitMap(screen1->ViewPort.RasInfo->BitMap,
             window1->LeftEdge+window1->BorderLeft,
             window1->TopEdge+window1->BorderTop,
             bmap,0,0,
             width,height,0xC0,0xFF,NULL);

   /* Copy bitplane pointers into local array "planes" */
   for (iPlane = 0; iPlane < depth; iPlane++)
      planes[iPlane] = (BYTE *)bmap->Planes[iPlane];

   /* Write out a BODY chunk header */
   error=PushChunk(iff, NULL, ID_BODY, IFFSIZE_UNKNOWN);
   if(error) {printf("PushChunk BODY error = %ld\n",error);
             PopChunk(iff);
             for(i=0;i<depth;i++) FreeRaster(bmap->Planes[i],width,height);
             FreeVec(bmap);
             CloseIFF(iff);
             Close(iff->iff_Stream);
             FreeIFF(iff);
             return;}

   /* Write out the BODY contents */
   for (iRow=height ; iRow>0 ; iRow--)
     {
     for (iPlane = 0; iPlane < depth; iPlane++)
        {
        /* write next row.*/
        if(WriteChunkBytes(iff,planes[iPlane],(LONG)bmap->BytesPerRow)!=
        bmap->BytesPerRow)
              {    
              printf("WriteChunkBytes BODY error.\n");
              PopChunk(iff);
              PopChunk(iff);
              for(i=0;i<depth;i++) FreeRaster(bmap->Planes[i],width,height);
              FreeVec(bmap);
              CloseIFF(iff);
              Close(iff->iff_Stream);
              FreeIFF(iff);
              return;
              }
        planes[iPlane] += (LONG)bmap->BytesPerRow;
        }
     }

   /* close out BODY chunk */
   PopChunk(iff);

   /* close out FORM chunk */
   PopChunk(iff);

   /* clean up */
   for(i=0;i<depth;i++) FreeRaster(bmap->Planes[i],width,height);
   FreeVec(bmap);
   CloseIFF(iff);
   Close(iff->iff_Stream);
   FreeIFF(iff);
}

/****************************************************************/

void canvas_size (void)
{
struct Requester size_req;
InitRequester(&size_req);  /* clear the structure */

size_req.Width=140;
size_req.Height=80;
size_req.ReqText=&itext5;
size_req.BackFill=31;
size_req.ReqGadget=&reqgad4;
size_req.ReqBorder=&border2;

si1.LongInt=window1->Width-window1->BorderLeft-window1->BorderRight;
si2.LongInt=window1->Height-window1->BorderTop-window1->BorderBottom;
/* we want numerical gadgets to show actual size of canvas */
sprintf(si1.Buffer,"%d",si1.LongInt);
sprintf(si2.Buffer,"%d",si2.LongInt);

Request(&size_req,window1);  /* show requester */
}

/****************************************************************/

void clean_exit(TEXT *str, int rcode)
{
	
    if(ras)
       FreeRaster(ras, wMaxX, wMaxY); /* Must free allocated temp memory
                                      from fills */
    if (strip) 
      {
          ClearMenuStrip(window1);
          ClearMenuStrip(window2);
          ClearMenuStrip(window3);
          FreeMenus(strip);
      }

    if (window1)       CloseWindow(window1);
    if (window2)       CloseWindow(window2);
    if (window3)       CloseWindow(window3);
    if (screen1)       CloseScreen(screen1);
    if (fr)            FreeAslRequest(fr);

    if (GadToolsBase)  CloseLibrary( GadToolsBase );
    if (IntuitionBase) CloseLibrary((struct Library *) IntuitionBase);
    if (DataTypesBase) CloseLibrary( DataTypesBase );
    if (AslBase)       CloseLibrary( AslBase );
    if (GfxBase)       CloseLibrary((struct Library *) GfxBase );
    if (IFFParseBase)  CloseLibrary( IFFParseBase );

    if (str)
    {
        puts(str);
    }
    exit(rcode);
}

/************** END OF RABLE.C *****************/
