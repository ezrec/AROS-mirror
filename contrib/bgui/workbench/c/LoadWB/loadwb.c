#include "common.h"
#include "strings.h"
#include "pattern.h"
#include "execute.h"
#include "about.h"

#define DEBUG 1
#include <aros/debug.h>

struct Library *BGUIBase;

#ifdef _AROS
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct DosLibrary *DOSBase;
#endif

/*** Defines *****************************************************************/

enum
{
    /* Gadgets */

    NUM_BUTTONS,

    /* Menu-ID */

    ID_EXECUTE,     /* "Execute command..." */
    ID_ABOUT,       /* "About"              */
    ID_QUIT         /* "Quit"               */
};


struct NewMenu menustrip[] =
{
    Title( getString( WBM_WORKBENCH ) ),
	Item( getString( WBM_EXECUTE ), getString( WBM_EXECUTE_SC ), ID_EXECUTE ),
	Item( getString( WBM_ABOUT ),   getString( WBM_ABOUT_SC ),   ID_ABOUT   ),
	ItemBar,
	Item( getString( WBM_QUIT ),    getString( WBM_QUIT_SC ),    ID_QUIT    ),
    End
};

/*** Global variables ********************************************************/

Object *WO_Window;

struct Window *mainwin;
struct Screen *screen;

APTR wlock;

static char tempstring[256];

/*** Prototypes **************************************************************/

static void Cleanup( char *msg );
static void OpenLibs( void );
static void CloseLibs( void );

static void MakeGUI( void );
static void OpenGUI( void );

static void HandleAll( void );

/*** Functions ***************************************************************/

/// static void Cleanup( char *msg )
static void Cleanup( char *msg )
{
    WORD rc;

    if( msg )
    {
	printf( "Workbench: %s\n", msg );
	rc = RETURN_WARN;
    }
    else
    {
	rc = RETURN_OK;
    }

    if( WO_Window ) DisposeObject( WO_Window );

    CloseLibs();

    exit( rc );
} /// Cleanup()
/// static void OpenLibs( void )
static void OpenLibs( void )
{
    if( !(IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 39L )) )
    {
	sprintf( tempstring, getString( ERR_OPENLIBRARY ), "intuition.library", 39 );
	Cleanup( tempstring );
    }
    if( !(GfxBase = (struct GfxBase *) OpenLibrary( "graphics.library", 39L )) )
    {
	sprintf( tempstring, getString( ERR_OPENLIBRARY ), "graphics.library", 39 );
	Cleanup( tempstring );
    }

    if( !(DOSBase = (struct DosLibrary *) OpenLibrary( "dos.library", 39L )) )
    {
	sprintf( tempstring, getString( ERR_OPENLIBRARY ), "dos.library", 39 );
	Cleanup( tempstring );
    }

    if( !(BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION )) )
    {
	sprintf( tempstring , getString( ERR_OPENLIBRARY ), BGUINAME, BGUIVERSION );
	Cleanup( tempstring );
    }
} /// OpenLibs()
/// static void CloseLibs( void )
static void CloseLibs( void )
{
    if( BGUIBase ) CloseLibrary( BGUIBase );
    if( DOSBase ) CloseLibrary( (struct Library *) DOSBase );
    if( GfxBase ) CloseLibrary( (struct Library *) GfxBase );
    if( IntuitionBase ) CloseLibrary( (struct Library *) IntuitionBase );
} /// CloseLibs()

/// static void MakeGUI( void )
static void MakeGUI( void )
{
    static struct bguiPattern pattern;
    static struct BitMap      patternbm;
    int i;

    /* It seems that AROS is missing something to make BGUI patterns 
     * work, as this just segfaults (works on AmigaOS though). 
     * bgui/BGUIDemo-->BackFill segfaults also when trying to use 
     * an other pattern...
     */

#if 1
    InitBitMap(&patternbm, PATTERN_DEPTH, PATTERN_WIDTH, PATTERN_HEIGHT);
    for(i = 0; i < PATTERN_DEPTH; i++)
    {
	patternbm.Planes[i] = (((UBYTE *)patterndata) + (i * PATTERN_PLANESIZE));
    }

    /* This is a bit hackish. Should really remap the bitmap properly instead,
       allocating pens and such. */

    screen = LockPubScreen( NULL );
    LoadRGB32(&screen->ViewPort, patterncoltab);
    UnlockPubScreen( NULL, screen );

    pattern.bp_Flags  = 0;
    pattern.bp_Left   = 0;
    pattern.bp_Top    = 0;
    pattern.bp_Width  = PATTERN_WIDTH;
    pattern.bp_Height = PATTERN_HEIGHT;
    pattern.bp_BitMap = &patternbm;
    pattern.bp_Object = NULL;
#endif

    WO_Window = WindowObject,
	WINDOW_SmartRefresh,  TRUE,
	WINDOW_AutoAspect,    TRUE,
	WINDOW_ShowTitle,     TRUE,
	WINDOW_Backdrop,      TRUE,
	WINDOW_CloseOnEsc,    TRUE,
	WINDOW_MenuStrip,     menustrip,
	WINDOW_ScreenTitle,  "AROS Workbench",

	WINDOW_MasterGroup,
      VGroupObject, NormalOffset, NormalSpacing, FRM_FillPattern, &pattern, FRM_Type, FRTYPE_NONE,
		    /* Nothing here, yet. */
		EndMember,
	    EndObject,
    EndObject;
} ///
/// static void OpenGUI( void )
static void OpenGUI( void )
{
    if( WO_Window )
    {
	if( !(mainwin = WindowOpen( WO_Window )) )
	{
	    Cleanup( "Couldn't open window!" );
	}
    }
} ///

/// static void HandleAll( void )
static void HandleAll( void )
{
    ULONG signal = 0, rc;
    BOOL  running = TRUE;

    GetAttr( WINDOW_SigMask, WO_Window, &signal );
    do
    {
	Wait( signal );
	while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE )
	{
	    switch( rc )
	    {
		case WMHI_CLOSEWINDOW:
		case ID_QUIT:
		    running = FALSE;
		    break;

		case ID_EXECUTE:
		    wlock = BGUI_LockWindow( mainwin );
		    ExecuteTask();
		    BGUI_UnlockWindow( wlock );
		    break;

		case ID_ABOUT:
		    wlock = BGUI_LockWindow( mainwin );
		    AboutTask();
		    BGUI_UnlockWindow( wlock );
		    break;
	    }
	}
    } while( running );
} ///

/// int main( void )
int main( void )
{
    OpenLibs();

    MakeGUI();
    OpenGUI();

    HandleAll();

    Cleanup( NULL );
    return 0;
} ///

