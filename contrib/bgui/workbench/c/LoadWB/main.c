#define AROS_ALMOST_COMPATIBLE

#include "common.h"
#include "strings.h"
#include "execute.h"
#include "about.h"

#define DEBUG 1
#include <aros/debug.h>

struct Library       *BGUIBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
struct DosLibrary    *DOSBase;
struct Library       *WorkbenchBase;

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


struct NewMenu menustrip[] = {
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

struct List VolumeList;

/*** Prototypes **************************************************************/

static void Cleanup( char *msg );
static void OpenLibs( void );
static void CloseLibs( void );
static void Initialize( void );

static void MakeGUI( void );
static void OpenGUI( void );

static void HandleAll( void );

static void ScanVolumes( void );

/*** Functions ***************************************************************/

/// static void Cleanup( char *msg )
static void Cleanup( char *msg ) {
    WORD rc;

    if( msg ) {
        printf( "Workbench: %s\n", msg );
        rc = RETURN_WARN;
    } else {
        rc = RETURN_OK;
    }

    if( WO_Window ) DisposeObject( WO_Window );

    /* Free the memory used by the Volume List */
    if( VolumeList.lh_Head != NULL ) {
        struct Node *node;

        ForeachNode( &VolumeList, node ) {
            Remove( node );

            if( node->ln_Name )
                FreeVec( node->ln_Name );

            FreeVec( node );
        }
    }

    CloseLibs();

    exit( rc );
} /// Cleanup()
/// static void OpenLibs( void )
static void OpenLibs( void ) {
    if( !(IntuitionBase = (struct IntuitionBase *) OpenLibrary( INTUITIONNAME, 39L )) ) {
        sprintf( tempstring, getString( ERR_OPENLIBRARY ), INTUITIONNAME, 39 );
        Cleanup( tempstring );
    }

    if( !(GfxBase = (struct GfxBase *) OpenLibrary( GRAPHICSNAME, 39L )) ) {
        sprintf( tempstring, getString( ERR_OPENLIBRARY ), GRAPHICSNAME, 39 );
        Cleanup( tempstring );
    }

    if( !(DOSBase = (struct DosLibrary *) OpenLibrary( DOSNAME, 39L )) ) {
        sprintf( tempstring, getString( ERR_OPENLIBRARY ), DOSNAME, 39 );
        Cleanup( tempstring );
    }

    if( !(WorkbenchBase = OpenLibrary( WORKBENCHNAME, 39L )) ) {
        sprintf( tempstring , getString( ERR_OPENLIBRARY ), WORKBENCHNAME, 39 );
        Cleanup( tempstring );
    }

    if( !(BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION )) ) {
        sprintf( tempstring , getString( ERR_OPENLIBRARY ), BGUINAME, BGUIVERSION );
        Cleanup( tempstring );
    }
} /// OpenLibs()
/// static void CloseLibs( void )
static void CloseLibs( void ) {
    if( BGUIBase ) CloseLibrary( BGUIBase );
    if( DOSBase ) CloseLibrary( (struct Library *) DOSBase );
    if( GfxBase ) CloseLibrary( (struct Library *) GfxBase );
    if( IntuitionBase ) CloseLibrary( (struct Library *) IntuitionBase );
    if( WorkbenchBase ) CloseLibrary( WorkbenchBase );
} /// CloseLibs()

static void Initialize( void ) {
    /* Initialize the volume list. */
    NEWLIST( &VolumeList );
}

/// static void MakeGUI( void )
static void MakeGUI( void ) {
    WO_Window = WindowObject,
        WINDOW_SmartRefresh,  TRUE,
        WINDOW_AutoAspect,    TRUE,
        WINDOW_ShowTitle,     TRUE,
        WINDOW_Backdrop,      TRUE,
        WINDOW_CloseOnEsc,    TRUE,
        WINDOW_MenuStrip,     menustrip,
        WINDOW_ScreenTitle,  "AROS Workbench",

        WINDOW_MasterGroup,
            VGroupObject, NormalOffset, NormalSpacing, FRM_Type, FRTYPE_NONE,
                /* Nothing here, yet. */
            EndMember,
        EndObject,
    EndObject;
}

static void OpenGUI( void ) {
    if( WO_Window ) {
        if( !(mainwin = WindowOpen( WO_Window )) ) {
            Cleanup( "Couldn't open window!" );
        }
    }
}

/// static void HandleAll( void )
static void HandleAll( void ) {
    ULONG signal = 0, rc;
    BOOL  running = TRUE;

    GetAttr( WINDOW_SigMask, WO_Window, &signal );
    do {
        Wait( signal );
        while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
            switch( rc ) {
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

static void ScanVolumes( void ) {
    struct Node    *node;
    struct DosList *dl;

    if( (dl = LockDosList( LDF_VOLUMES | LDF_READ )) ) {
        while( (dl = NextDosEntry( dl, LDF_VOLUMES )) != NULL ) {
            node = AllocVec( sizeof( struct Node ), MEMF_ANY | MEMF_CLEAR );

            if( node ) {
                node->ln_Name = AllocVec( strlen( dl->dol_DevName ) + 1, MEMF_ANY );

                if( !node ) {
                    FreeVec( node );
                    Cleanup( "No memory.\n" );
                }

                strcpy( node->ln_Name, dl->dol_DevName );

                AddTail( &VolumeList, node );
            }
        }

        UnLockDosList( LDF_VOLUMES | LDF_READ );

    } else {
        Cleanup( "Could not lock the DosList.\n" );
    }
}

/// int main( void )
int main( void ) {
    OpenLibs();

    Initialize();
    ScanVolumes();

    MakeGUI();
    OpenGUI();

    HandleAll();

    Cleanup( NULL );
    return 0;
} ///

