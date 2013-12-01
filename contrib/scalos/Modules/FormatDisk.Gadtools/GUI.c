// Gui.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
// #include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>

#include <intuition/screens.h>

#include <libraries/gadtools.h>
#include <graphics/rastport.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>


#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>

#include <string.h>
#include <stdio.h>

#include <defs.h>

#include "Format.h"
#define	FormatDisk_NUMBERS
#include STR(SCALOSLOCALE)

#include "GUI.h"
#include "Format_disk_rev.h"
#include <Year.h>

//-----------------------------------------------------------------------------

struct Window	     *PrepWnd = NULL;
struct Window	     *StatusWnd = NULL;
struct Gadget	     *PrepGadgets[11];		// Number of gadgets
struct Gadget	     *StatusGadgets[2];

char NewVolumeName[64];

Rect box;

//-----------------------------------------------------------------------------

extern struct GfxBase *GfxBase;

extern char volumeName[];
extern char deviceName[];

extern STRPTR ShortDostype;

//-----------------------------------------------------------------------------

static struct Screen	     *Scr = NULL;
static APTR		      VisualInfo = NULL;
static struct Gadget	     *PrepGList = NULL;
static struct Gadget	     *StatusGList = NULL;
static UWORD		      PrepLeft = 0;
static UWORD		      PrepTop = 23;
static UWORD		      PrepWidth = 332;
static UWORD		      PrepHeight = 90;
static UWORD		      StatusLeft = 0;
static UWORD		      StatusTop = 23;
static UWORD		      StatusWidth = 480;
static UWORD		      StatusHeight = 69;
static STRPTR		      PrepWdt;
static STRPTR		      StatusWdt;

static struct TextAttr      *Font, Attr;

static UWORD FontX, FontY;
static UWORD OffX, OffY;
static UWORD ScreenBarH;

static char StatusTextDrive[128];
static char StatusTextNSDSupport[256];

static BOOL DefaultFont=FALSE;

static UWORD SpaceChar;
static UWORD CheckBoxPixel;
static BOOL  ForceFontTopaz8 = FALSE;
static UWORD gd_StatusGadgetWidth;

//-----------------------------------------------------------------------------

static LONG GetLenForText(LONG LMsgig, LONG Current, struct TextFont *tf);	// Used with LONG
static LONG GetLenForText2(STRPTR LString, LONG Current, struct TextFont *tf);	// Used with STRPTR

static void InitScreenFont( void );

//-----------------------------------------------------------------------------

static struct IntuiText  StatusIText[] =
	{
	{1, 0, JAM1,3, 2, NULL, StatusTextDrive, &StatusIText[1] },
	{1, 0, JAM1,3, 12, NULL, StatusTextNSDSupport },
	{0}
        };

//-----------------------------------------------------------------------------

static UWORD ComputeX( UWORD value )
{
	return(( UWORD )(( FontX * value) / 8)); // / 8
}

//-----------------------------------------------------------------------------

static UWORD ComputeY( UWORD value )
{
	return(( UWORD )(( FontY * value ) / 8)); // 8
}

//-----------------------------------------------------------------------------
static void InitScreenFont( void )
{
	struct TextFont *tf;
	Font = &Attr;

	Font->ta_Name = Scr->Font->ta_Name;
	tf = OpenDiskFont(Scr->Font);		// OpenDiskFont
	ScreenBarH = tf->tf_YSize + Scr->WBorTop + 1;
	OffY  = ScreenBarH;
	CheckBoxPixel = OffY;
	CloseFont(tf);

	if (DefaultFont)
		{
		// Default font.
		Font->ta_Name = GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name;
		Font->ta_YSize = FontY = GfxBase->DefaultFont->tf_YSize;
		FontX = GfxBase->DefaultFont->tf_XSize;
		CheckBoxPixel = OffY;
		}
	else
		{
		// Screen font.
		tf = OpenDiskFont(Scr->Font);
		Font->ta_YSize = FontY = tf->tf_YSize;
		FontX = tf->tf_XSize;
		Font = Scr->Font;
		CloseFont(tf);
		}

	if (OffX > OffY)
		CheckBoxPixel = OffX;

    	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Font Name = <%s> X=%ld / Y=%ld ScreenBarH = %ld CheckBoxPixel= %ld \n", \
		__LINE__, Font->ta_Name, (LONG) FontX, (LONG) FontY, (LONG) OffY, (LONG) CheckBoxPixel));

	OffX = Scr->WBorLeft;
}

//-----------------------------------------------------------------------------

static BOOL ComputeFont( UWORD width, UWORD height )
{
	if ( width && height )
		{
		if (( width + OffX + Scr->WBorRight ) > Scr->Width )
			return TRUE;
			// goto UseTopaz;
		if (( height + OffY + Scr->WBorBottom ) > Scr->Height )
			return TRUE;
			// goto UseTopaz;
		}
	return FALSE;
}

//-----------------------------------------------------------------------------

int SetupScreen( void )
{
	if ( ! ( Scr = LockPubScreen( NULL )))
		return( 1L );

	ComputeFont( 0L, 0L );

	if ( ! ( VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
		return( 2L );

	if(Scr)
		{
		PrepTop    = (Scr->Height/2) - (PrepHeight/2) - (Scr->BarHeight*2);
		PrepLeft   = (Scr->Width/2) - (PrepWidth/2) + (((Scr->WBorLeft+4)*2)+1);

	    	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: PrepLeft =%ld\n", __LINE__, PrepLeft));

		StatusTop  = (Scr->Height/2) - (StatusHeight/2) - (Scr->BarHeight*2);
		StatusLeft = (Scr->Width/2) - (StatusWidth/2) + ((Scr->WBorLeft+4)*2);

	    	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: StatusLeft =%ld\n", __LINE__, StatusLeft));
		}

    return( 0L );
}

//-----------------------------------------------------------------------------

void CloseDownScreen( void )
{
	if (VisualInfo)
		{
		FreeVisualInfo( VisualInfo );
		VisualInfo = NULL;
		}

	if (Scr)
		{
		UnlockPubScreen( NULL, Scr );
		Scr = NULL;
	}
}

//-----------------------------------------------------------------------------

int OpenPrepWindow( char *volumeName , BOOL GetFFS, BOOL GetINTL, BOOL GetDirCache)
{
	struct TextFont *tf;
	struct NewGadget ng;
	struct Gadget *g;
	UWORD  wleft, wtop, ww, wh;
	UWORD  BtWidth;
	char StripVolumeName[64];
	char OnDeviceName[256];
	CONST_STRPTR gd_DeviceGad_Text;
	size_t volumeNamelen;
	size_t deviceNamelen;
	UWORD WestNameOrDevice;			// +jmc+
	UWORD Row1;				// +jmc+
	UWORD Row2;				// +jmc+
	UWORD Row3;				// +jmc+
	UWORD Row4;				// +jmc+
	UWORD Row5;				// +jmc+
	UWORD Row6;				// +jmc+
	UWORD West;				// +jmc+
	UWORD TrashCanFieldPosition;		// +jmc+
	UWORD WestOnDrive;			// +jmc+
	BOOL AddOnDrive = FALSE;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: deviceName=<%s>\n", __LINE__, deviceName));

	PrepWdt = (STRPTR) GetLocString(MSGID_GUI_PREPWINDOW_TITLE);

	volumeNamelen=strlen(volumeName);
	deviceNamelen=strlen(deviceName);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: volumeNamelen=<%ld> - deviceNamelen=<%ld>\n", __LINE__, volumeNamelen, deviceNamelen));

	stccpy(StripVolumeName, volumeName, volumeNamelen);

	InitScreenFont();

	tf = OpenDiskFont(Font);

	//================================================================================================================================

	if ( ! ( g = CreateContext( &PrepGList )))
		return( 1L );

	if (deviceNamelen == 0)
		{
		strcpy(OnDeviceName, volumeName);
		gd_DeviceGad_Text = GetLocString(MSGID_GADGET_DEVICE);
		Row1 =  GetLenForText(MSGID_GADGET_DEVICE, 0, tf);
		}
	else
		{
		sprintf(OnDeviceName, "%s (%s \"%s:\")", volumeName, GetLocString(MSGID_GUI_ON_DRIVE), deviceName);
		gd_DeviceGad_Text = GetLocString(MSGID_GADGET_VOLUME);
		Row1 =  GetLenForText(MSGID_GADGET_VOLUME, 0, tf);
		AddOnDrive = TRUE;
		}

	SpaceChar = FontX;

	West = Row1;
	WestNameOrDevice = Row1;

	Row2 =  GetLenForText(MSGID_GADGET_DEVNAME, 0, tf);
	if (Row2 > West)
		{
		West = Row2;
		WestNameOrDevice = Row2;
		}

	WestNameOrDevice = WestNameOrDevice + SpaceChar + (Scr->WBorLeft + Scr->WBorRight);

	Row3 =  GetLenForText(MSGID_GADGET_FFS, 0, tf);
	Row3 += GetLenForText(MSGID_GADGET_INTL, Row3, tf);
	if (Row3 > West)
		West = Row3;

	Row4 =  GetLenForText(MSGID_GADGET_VERIFY, 0, tf);
	Row4 += GetLenForText(MSGID_GADGET_TRASHCAN, Row4, tf);
	if (Row4 > West)
		West = Row4;

	Row5 =  GetLenForText(MSGID_GADGET_DIRCACHE, 0, tf);
	Row5 += GetLenForText(MSGID_GADGET_TRASHCAN_NAME, Row5, tf);
	if (Row5 > West)
		West = Row5;

	Row6 =  GetLenForText(MSGID_GADGET_GUI_FORMAT, 0, tf) + (SpaceChar * 2);
	Row6 += GetLenForText(MSGID_GADGET_QUICK_FORMAT, Row6, tf) + (SpaceChar * 2);
	Row6 += GetLenForText(MSGID_GADGET_GUI_CANCEL, Row6, tf) + (SpaceChar * 2);
	if (Row6 > West)
		West = Row6;


	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld:\nForceFontTopaz8 = [%ld]\nRow1 = %ld \nRow2 = %ld\nRow3 = %ld\nRow4 = %ld\nRow5 = %ld\nRow6 = %ld\n=> West = %ld\n", \
		__LINE__, ForceFontTopaz8, Row1, Row2, Row3, Row4, Row5, Row6, West));


	TrashCanFieldPosition = (West/2) + GetLenForText(MSGID_GADGET_TRASHCAN_NAME, 0, tf);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: West = [%ld]\n", __LINE__, West));

	if (AddOnDrive == TRUE)
		{
		WestOnDrive = GetLenForText2((STRPTR) OnDeviceName, 0,tf) + WestNameOrDevice;
		if (WestOnDrive > West)
			{
			West = WestOnDrive;
			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Change Widest: West = [%ld]\n", __LINE__, West));
			}
		}

	ww = West + (SpaceChar * 3);

	CloseFont(tf);

//---------------------------------- Check Window size according to Screen width and height ----------------------------------------------------------

	ForceFontTopaz8 = ComputeFont( ww, PrepHeight );

//---------------------------------- Window width size initialization once again ----------------------------------------------------------------------

	if (ForceFontTopaz8)
		{
		Font->ta_Name = (STRPTR)"topaz.font";
		FontX = FontY = Font->ta_YSize = 8;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: ForceFontTopaz8=[%ld] Font Name = <%s> X=%ld / Y=%ld ScreenBarH = %ld\n", \
			__LINE__, ForceFontTopaz8, Font->ta_Name, (LONG) FontX, (LONG) FontY, (LONG) OffY));

		tf = OpenDiskFont(Font);

		// wh = ComputeY( PrepHeight );

		if (deviceNamelen == 0)
			{
			strcpy(OnDeviceName, volumeName);
			gd_DeviceGad_Text = GetLocString(MSGID_GADGET_DEVICE);
			Row1 =  GetLenForText(MSGID_GADGET_DEVICE, 0, tf);
			}
		else
			{
			sprintf(OnDeviceName, "%s (On drive \"%s:\")", volumeName, deviceName);
			gd_DeviceGad_Text = GetLocString(MSGID_GADGET_VOLUME);
			Row1 =  GetLenForText(MSGID_GADGET_VOLUME, 0, tf);
			}

		SpaceChar = FontX;

		West = Row1;
		WestNameOrDevice = Row1;

		Row2 =  GetLenForText(MSGID_GADGET_DEVNAME, 0, tf);
		if (Row2 > West)
			{
			West = Row2;
			WestNameOrDevice = Row2;
			}

		WestNameOrDevice = WestNameOrDevice + SpaceChar + (Scr->WBorLeft + Scr->WBorRight);

		Row3 =  GetLenForText(MSGID_GADGET_FFS, 0, tf);
		Row3 += GetLenForText(MSGID_GADGET_INTL, Row3, tf);
		if (Row3 > West)
			West = Row3;

		Row4 =  GetLenForText(MSGID_GADGET_VERIFY, 0, tf);
		Row4 += GetLenForText(MSGID_GADGET_TRASHCAN, Row4, tf);
		if (Row4 > West)
			West = Row4;

		Row5 =  GetLenForText(MSGID_GADGET_DIRCACHE, 0, tf);
		Row5 += GetLenForText(MSGID_GADGET_TRASHCAN_NAME, Row5, tf);
		if (Row5 > West)
			West = Row5;

		Row6 =  GetLenForText(MSGID_GADGET_GUI_FORMAT, 0, tf) + (SpaceChar * 2);
		Row6 += GetLenForText(MSGID_GADGET_QUICK_FORMAT, Row6, tf) + (SpaceChar * 2);
		Row6 += GetLenForText(MSGID_GADGET_GUI_CANCEL, Row6, tf) + (SpaceChar * 2);
		if (Row6 > West)
			West = Row6;

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld:\nForceFontTopaz8 = [%ld]\nRow1 = %ld \nRow2 = %ld\nRow3 = %ld\nRow4 = %ld\nRow5 = %ld\nRow6 = %ld\n=> West = %ld\n", \
			__LINE__, ForceFontTopaz8, Row1, Row2, Row3, Row4, Row5, Row6, West));


		TrashCanFieldPosition = (West/2) + GetLenForText(MSGID_GADGET_TRASHCAN_NAME, 0, tf);

		if (AddOnDrive == TRUE)
			{
			WestOnDrive = GetLenForText2((STRPTR) OnDeviceName, 0,tf) + WestNameOrDevice;
			if (WestOnDrive > West)
				{
				West = WestOnDrive;
				d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Change Widest: West = [%ld]\n", __LINE__, West));
				}
			}

		ww = West + (SpaceChar * 2);

		CloseFont(tf);
		}

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: AddOnDrive = [%ld] - West = [%ld] Window width = [%ld] \n", __LINE__, AddOnDrive, West, ww));

	wh = ComputeY( PrepHeight );

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: wh = [%ld] OffY = %ld ScreenBarH = %ld\n", __LINE__, wh, OffY, ScreenBarH));

//- ROW 1 ----------------------------------------------------------------------------------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + WestNameOrDevice;
	ng.ng_TopEdge	  =  ScreenBarH + ComputeY( 4 );
	ng.ng_Width 	  =  ww - ((OffX*2) + WestNameOrDevice) - Scr->WBorLeft - Scr->WBorRight;
	ng.ng_Height	  =  ComputeY( 13 );
	ng.ng_TextAttr	  =  Font;
	ng.ng_GadgetText  =  (STRPTR) gd_DeviceGad_Text;							// TEXT Gadget: "Volume:" or "Device:"
	ng.ng_GadgetID	  =  GD_DeviceGad;
	ng.ng_Flags 	  =  PLACETEXT_LEFT;
	ng.ng_VisualInfo  =  VisualInfo;

	g = CreateGadget( TEXT_KIND,
		 g, &ng,
		 GTTX_Text, OnDeviceName,
		 GTTX_Border, TRUE,
		 TAG_DONE );

	PrepGadgets[ GDX_DeviceGad ] = g;

	//---------------------------------------------------------------------

//- ROW 2 ----------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + WestNameOrDevice;
	ng.ng_TopEdge	  =  ScreenBarH + ComputeY( 18 );
	ng.ng_Width 	  =  ww - ((OffX*2) + WestNameOrDevice) - Scr->WBorLeft - Scr->WBorRight;
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_DEVNAME);					// STRING Gadget: "Name"
	ng.ng_GadgetID	  =  GD_NameGadget;
	ng.ng_Flags 	  =  PLACETEXT_LEFT;

	g = CreateGadget( STRING_KIND, g, &ng, GTST_String, StripVolumeName,
		 GTST_MaxChars, 30,
		 GT_Underscore, '_',
		 STRINGA_Justification, GACT_STRINGCENTER,
		 TAG_DONE );

	PrepGadgets[ GDX_NameGadget ] = g;

	//---------------------------------------------------------------------

//- ROW 3 ----------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + ComputeX( 5 );
	ng.ng_TopEdge	  =  ScreenBarH + ComputeY( 33 );
	ng.ng_Width 	  =  CheckBoxPixel;
	ng.ng_Height	  =  CheckBoxPixel;
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_FFS);						// CHECKBOX Gadget: "Fast File System"
	ng.ng_GadgetID	  =  GD_FFSGadget;
	ng.ng_Flags 	  =  PLACETEXT_RIGHT;

	g = CreateGadget( CHECKBOX_KIND,
		 g, &ng,
		 GTCB_Checked, GetFFS,
		 GT_Underscore, '_',
		 GA_Disabled, !GetFFS,
		 TAG_DONE );

	PrepGadgets[ GDX_FFSGadget ] = g;

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  ww/2;
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_INTL);						// CHECKBOX Gadget: "INTL" (Internationnal)
	ng.ng_GadgetID	  =  GD_IntlGadget;

	g = CreateGadget( CHECKBOX_KIND,
		 g, &ng,
		 GTCB_Checked, GetINTL,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_IntlGadget ] = g;

	//---------------------------------------------------------------------

//- ROW 4 ----------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + ComputeX( 5 );
	ng.ng_TopEdge	  =  ScreenBarH + ComputeY( 46 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_VERIFY);					// CHECKBOX Gadget: "Verify Format".
	ng.ng_GadgetID	  =  GD_VerifyGadget;

	g = CreateGadget( CHECKBOX_KIND, g, &ng, GTCB_Checked, Verify, GT_Underscore, '_', TAG_DONE );

	PrepGadgets[ GDX_VerifyGadget ] = g;

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  ww/2;
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_TRASHCAN);					// CHECKBOX Gadget: "With Traschcan".
	ng.ng_GadgetID	  =  GD_IconGadget;
	ng.ng_Flags 	  =  PLACETEXT_RIGHT;

	g = CreateGadget( CHECKBOX_KIND,
		 g, &ng,
		 GTCB_Checked, Icon,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_IconGadget ] = g;


	//---------------------------------------------------------------------

//- ROW 5 ----------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + ComputeX( 5 );
	ng.ng_TopEdge	  =  ScreenBarH + ComputeY( 60 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_DIRCACHE);					// CHECKBOX Gadget "DirCache".
	ng.ng_GadgetID	  =  GD_DirCacheGadget;
	ng.ng_Flags 	  =  PLACETEXT_RIGHT;

	g = CreateGadget( CHECKBOX_KIND,
		 g, &ng,
		 GTCB_Checked, GetDirCache,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_DirCacheGadget ] = g;

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  TrashCanFieldPosition;
	ng.ng_TopEdge	  -= ComputeY( 2 );
	ng.ng_Width 	  =  ww - TrashCanFieldPosition - Scr->WBorLeft - Scr->WBorRight - 3;
	ng.ng_Height	  =  ComputeY( 13 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_TRASHCAN_NAME);					// STRING Gadget "Trashcan Name".
	ng.ng_GadgetID	  =  GD_TrashCanNameGadget;
	ng.ng_Flags 	  =  PLACETEXT_LEFT;

	g = CreateGadget( STRING_KIND,
		 g, &ng,
		 GTST_String, "Trashcan",
		 GTST_MaxChars, 25,
		 GT_Underscore, '_',
		 GA_Disabled, !Icon,
		 TAG_DONE );

	PrepGadgets[ GDX_TrashCanNameGadget ] = g;

	//---------------------------------------------------------------------

//- ROW 6 ----------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------


	BtWidth = ( (ww - Scr->WBorRight - Scr->WBorLeft - (FontX+4)) / 3 );

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Bouton Width = %ld ww = %ld\n", __LINE__, BtWidth, ww));

	ng.ng_LeftEdge	  =  OffX + ComputeX( 4 );
	ng.ng_TopEdge	  =  OffY + ComputeY( 76 );
	ng.ng_Width 	  =  BtWidth;
	ng.ng_Height	  =  ComputeY( 12 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_GUI_FORMAT);					    // BUTTON Gadget: "OK".
	ng.ng_GadgetID	  =  GD_OKGadget;
	ng.ng_Flags 	  =  PLACETEXT_IN;

	g = CreateGadget( BUTTON_KIND,
		 g, &ng,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_OKGadget ] = g;

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + ComputeX( 4 ) + (BtWidth + 2) ; 
	ng.ng_Width 	  =  BtWidth;
	ng.ng_Height	  =  ComputeY( 12 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_QUICK_FORMAT);					// BUTTON Gadget: "Quick Format".
	ng.ng_GadgetID	  =  GD_QuickFmtGadget;
	ng.ng_Flags 	  =  PLACETEXT_IN;

	g = CreateGadget( BUTTON_KIND,
		 g, &ng,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_QuickFmtGadget ] = g;

	//---------------------------------------------------------------------

	ng.ng_LeftEdge	  =  OffX + ComputeX( 4 ) + (BtWidth * 2) + 4;
	ng.ng_Width 	  =  BtWidth;
	ng.ng_Height	  =  ComputeY( 12 );
	ng.ng_GadgetText  =  (STRPTR) GetLocString(MSGID_GADGET_GUI_CANCEL);					// BUTTON Gadget: "Cancel".
	ng.ng_GadgetID	  =  GD_CancelGadget;
	ng.ng_Flags 	  =  PLACETEXT_IN;

	g = CreateGadget( BUTTON_KIND,
		 g, &ng,
		 GT_Underscore, '_',
		 TAG_DONE );

	PrepGadgets[ GDX_CancelGadget ] = g;


	//---------------------------------------------------------------------

	if ( ! g )
		return( 2L );


	wleft = ((Scr->Width/2) - ((ww + Scr->WBorRight + Scr->WBorLeft)/2));
	wtop  = ((Scr->Height/2) - ((wh + Scr->BarHeight + Scr->WBorTop + Scr->WBorBottom)/2));

	if ( ! ( PrepWnd = OpenWindowTags( NULL,
		WA_Left,	wleft,
		WA_Top,	      	wtop,
		WA_Width,	ww,
		WA_Height,	wh + OffY + Scr->WBorBottom,
		WA_IDCMP,	IDCMP_VANILLAKEY|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|BUTTONIDCMP|CHECKBOXIDCMP,
		WA_Flags,	WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_RMBTRAP,
		WA_Gadgets,     PrepGList,
		WA_Title,	PrepWdt,
		WA_ScreenTitle, TITLE_NAME,
		WA_PubScreen,   Scr,
		TAG_DONE )))

	return( 4L );

	GT_RefreshWindow( PrepWnd, NULL );

	return( 0L );
}

//-----------------------------------------------------------------------------

void ClosePrepWindow( void )
{
	if ( PrepWnd )
		{
		CloseWindow( PrepWnd );
		PrepWnd = NULL;
		}

	if ( PrepGList )
		{
		FreeGadgets( PrepGList );
		PrepGList = NULL;
		}
}

//-----------------------------------------------------------------------------

void StatusRender( void )
{
	ULONG leftEdge[2],topEdge[2];
	ComputeFont( StatusWidth, StatusHeight );

	leftEdge[0] = StatusIText[0].LeftEdge;
	leftEdge[1] = StatusIText[1].LeftEdge;

	topEdge[0] = StatusIText[0].TopEdge;
	topEdge[1] = StatusIText[1].TopEdge;


	StatusIText[0].LeftEdge = ComputeX(StatusIText[0].LeftEdge);
	StatusIText[0].TopEdge = ComputeY(StatusIText[0].TopEdge);
	StatusIText[1].LeftEdge = ComputeX(StatusIText[1].LeftEdge);
	StatusIText[1].TopEdge = ComputeY(StatusIText[1].TopEdge);

	PrintIText(StatusWnd->RPort,StatusIText,OffX,OffY);

	StatusIText[0].LeftEdge = leftEdge[0];
	StatusIText[0].TopEdge = topEdge[0];
	StatusIText[1].LeftEdge = leftEdge[1];
	StatusIText[1].TopEdge = topEdge[1];

	DrawBevelBox( StatusWnd->RPort, OffX + ComputeX( 3 ),
		OffY + ComputeY( 38 ),
		gd_StatusGadgetWidth - (ComputeX( 3 ) * 2),
		ComputeY( 13 ),
		GT_VisualInfo, VisualInfo, GTBB_Recessed, TRUE,
		TAG_DONE );

	box.left = OffX+ComputeX(3);
	box.top = OffY+ComputeY(38);
	box.width = gd_StatusGadgetWidth - (ComputeX( 3 ) * 2);
	box.height = ComputeY(13);
	box.center = box.left+box.width/2;
}

//-----------------------------------------------------------------------------

int OpenStatusWindow(char *bufPointer)
{
	struct NewGadget ng;
	struct Gadget *g;
	UWORD  wleft, wtop, ww, wh;
	struct TextFont *tf;
	UWORD Row1;				// +jmc+
	UWORD Row2;				// +jmc+
	UWORD Row3;				// +jmc+
	UWORD Row3b;				// +jmc+
	UWORD Row4;				// +jmc+
	UWORD West;				// +jmc+


	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Volume=<%s>\n", __LINE__, volumeName));

	tf = OpenDiskFont(Font);

	if ( ! ( g = CreateContext( &StatusGList )))
		return( 1L );

	StatusWdt = (STRPTR) GetLocString(MSGID_GUI_STATUSWINDOW_TITLE);

	sprintf(StatusTextDrive, GetLocString(MSGID_GUI_STATUSWINDOW_FORMATTING_DRIVE), volumeName, NewVolumeName, ShortDostype);
	Row1 = GetLenForText2((STRPTR) StatusTextDrive, 0,tf);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: LENGHT: Row1=[%ld] \n", __LINE__, Row1));

	SpaceChar = FontX;

	West = Row1;
	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: --> \"%s\" as \"%s\"...\n", __LINE__, volumeName, NewVolumeName));


	if (NsdSupport==TRUE)
		sprintf(StatusTextNSDSupport, GetLocString(MSGID_GUI_STATUSWINDOW_DEVICE_64BIT), deviceName);
	else
		sprintf(StatusTextNSDSupport, GetLocString(MSGID_GUI_STATUSWINDOW_DEVICE_NO64BIT), deviceName);

	Row2 = GetLenForText2((STRPTR) StatusTextNSDSupport, 0,tf);

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: LENGHT: Row2=[%ld]\n", __LINE__, Row2));

	if (Row2 > West)
		West = Row2;

//------ Get string lenght of "Formatting" and "Verifying" strings -------------------

	Row3 = GetLenForText(MSGID_GUI_FORMATTING_CYLINDER, 0, tf)  + (SpaceChar * 4);
	if (Row3 > West)
		West = Row3;

	Row3b = GetLenForText(MSGID_GUI_VERIFYING_CYLINDER, 0, tf)  + (SpaceChar * 4);
	if (Row3b > West)
		West = Row3b;

//-------------------------------------------------------------------------------------

	Row4 = GetLenForText(MSGID_GUI_STATUSWINDOW_ABORT, 0, tf)  + (SpaceChar * 2);

	if (Row4 > West)
		West = Row4;

	ww = West + SpaceChar + Scr->WBorRight + Scr->WBorLeft;

	CloseFont(tf);

//---------------------------------- Check Window size according to Screen width and height ----------------------------------------------------------

	ForceFontTopaz8 = ComputeFont( ww, StatusHeight );

//---------------------------------- Window width size initialization once again ----------------------------------------------------------------------

	if (ForceFontTopaz8)
		{
		Font->ta_Name = (STRPTR)"topaz.font";
		FontX = FontY = Font->ta_YSize = 8;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: ForceFontTopaz8=[%ld] Font Name = <%s> X=%ld / Y=%ld ScreenBarH = %ld\n", \
			__LINE__, ForceFontTopaz8, Font->ta_Name, (LONG) FontX, (LONG) FontY, (LONG) OffY));

		tf = OpenDiskFont(Font);

		StatusWdt = (STRPTR) GetLocString(MSGID_GUI_STATUSWINDOW_TITLE);

		sprintf(StatusTextDrive, GetLocString(MSGID_GUI_STATUSWINDOW_FORMATTING_DRIVE), volumeName, NewVolumeName, ShortDostype);
		Row1 = GetLenForText2((STRPTR) StatusTextDrive, 0,tf);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: LENGHT: Row1=[%ld] \n", __LINE__, Row1));

		SpaceChar = FontX;

		West = Row1;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: --> \"%s\" as \"%s\"...\n", __LINE__, volumeName, NewVolumeName));

		if (NsdSupport==TRUE)
			sprintf(StatusTextNSDSupport, GetLocString(MSGID_GUI_STATUSWINDOW_DEVICE_64BIT), deviceName);
		else
			sprintf(StatusTextNSDSupport, GetLocString(MSGID_GUI_STATUSWINDOW_DEVICE_NO64BIT), deviceName);

		Row2 = GetLenForText2((STRPTR) StatusTextNSDSupport, 0,tf);

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: LENGHT: Row2=[%ld]\n", __LINE__, Row2));

		if (Row2 > West)
			West = Row2;

		//------ Get string lenght of "Formatting" and "Verifying" strings -------------------

		Row3 = GetLenForText(MSGID_GUI_FORMATTING_CYLINDER, 0, tf)  + (SpaceChar * 6);
		if (Row3 > West)
			West = Row3;

		Row3b = GetLenForText(MSGID_GUI_VERIFYING_CYLINDER, 0, tf)  + (SpaceChar * 6);
		if (Row3b > West)
			West = Row3b;

		//-------------------------------------------------------------------------------------

		Row4 = GetLenForText(MSGID_GUI_STATUSWINDOW_ABORT, 0, tf)  + (SpaceChar * 2);

		if (Row4 > West)
			West = Row4;

		ww = West + SpaceChar + Scr->WBorRight + Scr->WBorLeft;

		CloseFont(tf);
		}

	wh = ComputeY( StatusHeight );

	StatusWidth = ww;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: NsdSupport=%ld TextNSDSupport=<%s>\n", __LINE__, NsdSupport, StatusTextNSDSupport));

//- ROW3 - Status gadget - (Formatting / Verifying) ----------------------------------------------------------------------------------------------------

	gd_StatusGadgetWidth = ww - Scr->WBorRight - Scr->WBorLeft;

	ng.ng_LeftEdge	  =    OffX + ComputeX( 3 );
	ng.ng_TopEdge	  =    OffY + ComputeY( 22 );
	ng.ng_Width 	  =    gd_StatusGadgetWidth - (ComputeX( 3 ) * 2);
	ng.ng_Height	  =    ComputeY( 13 );
	ng.ng_GadgetText  =    NULL;
	ng.ng_TextAttr	  =    Font;
	ng.ng_GadgetID	  =    GD_StatusGadget;
	ng.ng_Flags 	  =    0;
	ng.ng_VisualInfo  =    VisualInfo;

	g = CreateGadget( TEXT_KIND, g, &ng, GTTX_Border, TRUE, TAG_DONE );

	StatusGadgets[ 0 ] = g;

//- ROW 4 - "_Stop" button gadget -----------------------------------------------------------------------------------------------------------------------

	ng.ng_LeftEdge	  =    (ww/2) - (Row4/2);
	ng.ng_TopEdge	  =    OffY + ComputeY( 54 );
	ng.ng_Width 	  =    Row4;
	ng.ng_GadgetText  =    (STRPTR) GetLocString(MSGID_GUI_STATUSWINDOW_ABORT);
	ng.ng_GadgetID	  =    GD_StopGadget;
	ng.ng_Flags 	  =    PLACETEXT_IN;

	g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

	StatusGadgets[ 1 ] = g;

	if ( ! g )
		return( 2L );

	wleft = ((Scr->Width/2) - ((ww + Scr->WBorRight + Scr->WBorLeft)/2));
	wtop  = ((Scr->Height/2) - ((wh + Scr->BarHeight + Scr->WBorTop + Scr->WBorBottom)/2));

	if ( ! ( StatusWnd = OpenWindowTags( NULL,
			WA_Left,	wleft,
			WA_Top,		wtop,
			WA_Width,	ww,
			WA_Height,	wh + OffY + Scr->WBorBottom,
			WA_IDCMP,	IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_VANILLAKEY|BUTTONIDCMP,
			WA_Flags,	WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_SMART_REFRESH,
			WA_Gadgets,	StatusGList,
			WA_Title,	StatusWdt,
			WA_ScreenTitle,	TITLE_NAME,
			WA_PubScreen,	Scr,
			TAG_DONE )))
		return( 4L );

	GT_RefreshWindow( StatusWnd, NULL );

	tf = OpenDiskFont(Font);	// +jmc+
	SetFont(StatusWnd->RPort, tf);	// +jmc+
	StatusRender();
	CloseFont(tf);			// +jmc+

	return( 0L );
}

//-----------------------------------------------------------------------------

void CloseStatusWindow( void )
{
	if ( StatusWnd )
		{
		CloseWindow( StatusWnd );
		StatusWnd = NULL;
		}

	if ( StatusGList )
		{
		FreeGadgets( StatusGList );
		StatusGList = NULL;
		}
}

//-----------------------------------------------------------------------------

static LONG GetLenForText(LONG LMsgid, LONG Current, struct TextFont *tf)
{
	CONST_STRPTR  pString;	/* pointer to the string retrieved from the locale table */
	LONG	lPixlen;	/* The pixel length of the string */
	struct RastPort rast;

	InitRastPort(&rast);
	SetFont(&rast, tf);


	pString = GetLocString(LMsgid);

	lPixlen = TextLength(&rast, (STRPTR) pString, strlen(pString));

	if (lPixlen > Current)
		Current = lPixlen;

	return Current;
}

static LONG GetLenForText2(STRPTR LString, LONG Current, struct TextFont *tf)
{
	LONG	lPixlen;	/* The pixel length of the string */
	struct RastPort rast;

	InitRastPort(&rast);
	SetFont(&rast, tf);


	lPixlen = TextLength(&rast, LString, strlen(LString));

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: Current=[%ld] LString len=[%ld] lPixlen=[%ld]\n", __LINE__, Current, strlen(LString), lPixlen));

	if (lPixlen > Current)
		Current = lPixlen;

	return Current;
}

