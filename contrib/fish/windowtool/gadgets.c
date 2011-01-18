#include <string.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/displayinfo.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <aros/debug.h>

#define GD_Gadget0                             0
#define GD_Gadget1                             1
#define GD_Gadget2                             2
#define GD_Gadget3                             3
#define GD_Gadget4                             4
#define GD_Gadget5                             5
#define GD_Gadget6                             6
#define GD_Gadget7                             7
#define GD_Gadget8                             8
#define GD_Gadget9                             9
#define GD_Gadget10                            10
#define GD_Gadget11                            11
#define GD_GadgetSave                            50		/* Save */
#define GD_GadgetHide                            51		/* Hide */
#define GD_GadgetQuit                            52		/* Quit */
#define GD_GadgetCancel                          53		/* Cancel */

#define KEYNUMMER 12
extern char keys[KEYNUMMER][256];

int windowopen=FALSE;
char windowtitle[80]="WindowTool 1.0 : Hotkey = ";	/* 25 */

struct Window        *Wnd        = 0l;
struct Screen        *Scr        = 0l;
APTR                  VisualInfo = 0l;
struct Gadget        *GList      = 0l;
struct Gadget        *Gadgets[16];
struct Menu          *Menus      = 0l;
UWORD                 Zoom[4]    = { 0, 0, 200, 11 };

struct TextAttr topaz8 = {
    ( STRPTR )"topaz.font", 8, 0x00, 0x01 };

struct TagItem    WindowTags[] = {
    {WA_Left,          -10},
    {WA_Top,           -10},
    {WA_Width,         610},
    {WA_Height,        138},
    {WA_IDCMP,         IDCMP_GADGETUP|IDCMP_MENUPICK|IDCMP_VANILLAKEY|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_CHANGEWINDOW},
    {WA_Flags,         WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_ACTIVATE},
    {WA_Gadgets,       0l},
    {WA_Title,         (IPTR)windowtitle},
    {WA_MinWidth,      200},
    {WA_MinHeight,     11},
    {WA_MaxWidth,      611},
    {WA_MaxHeight,     137},
    {WA_Zoom,          (Tag)Zoom},
    {TAG_DONE }};

struct IntuiText  IText[] = { {
    2, 0, JAM1,45, 17, &topaz8, (UBYTE *)"Hotkeys:", NULL } };

struct NewMenu NewMenu[] = {
    {NM_TITLE, "Project", 0l, 0, 0, 0l},
    {NM_ITEM, "New", "N", 0, 0, 0l},
    {NM_ITEM, "Open...", "O", 0, 0, 0l},
    {NM_ITEM, "Open .Info", 0, 0, 0, 0l},
    {NM_ITEM, NM_BARLABEL, 0l, 0, 0l, 0l},
    {NM_ITEM, "Save", "S", 0, 0, 0l},
    {NM_ITEM, "Save As...", "A", 0, 0, 0l},
    {NM_ITEM, "Save As .info",0, 0, 0, 0l},
    {NM_ITEM, NM_BARLABEL, 0l, 0, 0l, 0l},
    {NM_ITEM, "About...", 0, 0, 0, 0l},
    {NM_ITEM, NM_BARLABEL, 0l, 0, 0l, 0l},
    {NM_ITEM, "Hide", "H", 0, 0, 0l},
    {NM_ITEM, "Cancel", "C", 0, 0, 0l},
    {NM_ITEM, NM_BARLABEL, 0l, 0, 0l, 0l},
    {NM_ITEM, "Quit...", "Q", 0, 0, 0l},
    {NM_END, 0l, 0l, 0, 0l, 0l }};

long InitStuff( void )
{
    struct NewGadget     ng;
    struct Gadget       *g;

	if(windowopen==TRUE)
	{
		WindowToFront(Wnd);
		ActivateWindow(Wnd);
		return(0);
	}

    if ( NOT( Scr = LockPubScreen( "Workbench" )))
        return( 1l );

    if ( NOT( VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
        return( 2l );

    if ( NOT( g = CreateContext( &GList )))
        return( 3l );

    ng.ng_LeftEdge        =    110;
    ng.ng_TopEdge         =    29;
    ng.ng_Width           =    173;
    ng.ng_Height          =    13;
    ng.ng_GadgetText      =    "St_op";
    ng.ng_TextAttr        =    &topaz8;
    ng.ng_GadgetID        =    GD_Gadget0;
    ng.ng_Flags           =    PLACETEXT_LEFT;
    ng.ng_VisualInfo      =    VisualInfo;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[0], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 0 ] = g;

    ng.ng_TopEdge         =    44;
    ng.ng_GadgetText      =    "_Newshell";
    ng.ng_GadgetID        =    GD_Gadget1;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[1], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 1 ] = g;

    ng.ng_TopEdge         =    59;
    ng.ng_GadgetText      =    "Q_uit";
    ng.ng_GadgetID        =    GD_Gadget2;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[2], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 2 ] = g;

    ng.ng_TopEdge         =    74;
    ng.ng_GadgetText      =    "C_ycle";
    ng.ng_GadgetID        =    GD_Gadget3;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[3], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 3 ] = g;

    ng.ng_TopEdge         =    89;
    ng.ng_GadgetText      =    "Wo_rkbench";
    ng.ng_GadgetID        =    GD_Gadget4;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[4], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 4 ] = g;

    ng.ng_TopEdge        =    104;
    ng.ng_GadgetText      =    "C_loseWindow";
    ng.ng_GadgetID        =    GD_Gadget5;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[5], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 5 ] = g;

    ng.ng_LeftEdge        =    409;
    ng.ng_TopEdge         =    29;
    ng.ng_GadgetText      =    "_ActivateFront";
    ng.ng_GadgetID        =    GD_Gadget6;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[6], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 6 ] = g;

    ng.ng_TopEdge         =    44;
    ng.ng_GadgetText      =    "Make_Big";
    ng.ng_GadgetID        =    GD_Gadget7;

    g = CreateGadget( STRING_KIND, g, &ng, GTST_String,keys[7],GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 7 ] = g;

    ng.ng_TopEdge         =    59;
    ng.ng_GadgetText      =    "MakeS_mall";
    ng.ng_GadgetID        =    GD_Gadget8;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[8], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 8 ] = g;

    ng.ng_TopEdge         =    74;
    ng.ng_GadgetText      =    "_Zip";
    ng.ng_GadgetID        =    GD_Gadget9;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[9], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 9 ] = g;

    ng.ng_TopEdge         =    89;
    ng.ng_GadgetText      =    "Sho_w";
    ng.ng_GadgetID        =    GD_Gadget10;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[10], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 10 ] = g;

    ng.ng_GadgetText      =    "Ac_tiveToFront";
    ng.ng_TopEdge         =    104;
    ng.ng_GadgetID        =    GD_Gadget11;

    g = CreateGadget( STRING_KIND, g, &ng,GTST_String,keys[11], GTST_MaxChars, 256, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 11 ] = g;

    ng.ng_LeftEdge        =    110;
    ng.ng_TopEdge         =    120;
    ng.ng_Width           =    83;
    ng.ng_GadgetText      =    "_Save";
    ng.ng_GadgetID        =    GD_GadgetSave;
    ng.ng_Flags           =    PLACETEXT_IN;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 12 ] = g;

    ng.ng_LeftEdge        =    201;
    ng.ng_GadgetText      =    "_Hide";
    ng.ng_GadgetID        =    GD_GadgetHide;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 13 ] = g;

    ng.ng_LeftEdge        =    381;
    ng.ng_GadgetText      =    "_Quit";
    ng.ng_GadgetID        =    GD_GadgetQuit;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 14 ] = g;

    ng.ng_LeftEdge        =    292;
    ng.ng_GadgetText      =    "_Cancel";
    ng.ng_GadgetID        =    GD_GadgetCancel;

    g = CreateGadget( BUTTON_KIND, g, &ng, GT_Underscore, '_', TAG_DONE );

    Gadgets[ 15 ] = g;

    if ( NOT g )
        return( 4l );

    WindowTags[ 6 ].ti_Data = (LONG)GList;

    if ( NOT( Menus = CreateMenus( NewMenu, GTMN_FrontPen, 0l, TAG_DONE )))
        return( 6l );

    LayoutMenus( Menus, VisualInfo, GTMN_TextAttr, &topaz8, TAG_DONE );

	if(WindowTags[0].ti_Data==-10)	/* Bei -10 1.Aufruf und in die mitte */
	{																				/* setzen */
		WindowTags[0].ti_Data=(Scr->Width-WindowTags[2].ti_Data)/2;
		WindowTags[1].ti_Data=(Scr->Height-WindowTags[3].ti_Data)/2;
	}

	strcpy(&windowtitle[26],keys[10]);

    if ( NOT( Wnd = OpenWindowTagList( 0l, WindowTags )))
        return( 5l );

    SetMenuStrip( Wnd, Menus );
    GT_RefreshWindow( Wnd, 0l );

	PrintIText( Wnd->RPort, IText, 0l, 0l );

	windowopen=TRUE;

	return( 0l );
}

void CleanStuff( void )
{
	if(windowopen==FALSE)return;
	
    if ( Menus      ) {
        ClearMenuStrip( Wnd );
        FreeMenus( Menus );
    }

    if ( Wnd        )
		{
		    WindowTags[0].ti_Data = (LONG)Wnd->LeftEdge;
		    WindowTags[1].ti_Data = (LONG)Wnd->TopEdge;
	   	 WindowTags[2].ti_Data = (LONG)Wnd->Width;
		    WindowTags[3].ti_Data = (LONG)Wnd->Height;
        CloseWindow( Wnd );
	Wnd = NULL;
		}

    if ( GList      )
        FreeGadgets( GList );

    if ( VisualInfo )
        FreeVisualInfo( VisualInfo );

    if ( Scr        )
        UnlockPubScreen( 0l, Scr );
	windowopen=FALSE;
}
