#include "common.h"
#include "strings.h"
#include "developers.h"

/*** Prototypes *************************************************************/

void AboutTask( void );
void ReadCredits( void );
STRPTR readFile( STRPTR filename, LONG *size );

/*** Global variables *******************************************************/

char *credits[];
char *buffer;

/*** Local variables ********************************************************/

const enum {
    ID_OK,
    ID_TABS,
    ID_LICENSE
};

/*** Public functions *******************************************************/

void AboutTask( void ) {
    Object        *WO_About, *GO_Tabs, *GO_Pages;
    struct Window *win;
    ULONG          rc, signal = 0;
    BOOL           running    = TRUE;

    char          *pagelabels[] = { "About", "Developers", NULL };
    ULONG          tabs2pages[] = { MX_Active, PAGE_Active, TAG_END };

    WO_About = WindowObject,
        WINDOW_SmartRefresh, TRUE,
        WINDOW_AutoAspect,   TRUE,
        WINDOW_CloseOnEsc,   TRUE,
        WINDOW_RMBTrap,      TRUE,
        WINDOW_CloseGadget, FALSE,
        WINDOW_ScaleWidth,     15,
        WINDOW_ScaleHeight,    20,
        WINDOW_Title,          getString( ABT_WINDOWTITLE ),

        WINDOW_MasterGroup,
            VGroupObject, NormalOffset, NormalSpacing, GROUP_BackFill, SHINE_RASTER,
                StartMember,
                    VGroupObject, Offset( 0 ), Spacing( 0 ),
                        StartMember,
                            GO_Tabs = MxObject,
                                MX_TabsObject,  TRUE,
                                MX_Labels,         pagelabels,
                                MX_Active,         0,
                                GA_ID,             ID_TABS,
                                GROUP_BackFill,    SHINE_RASTER,
                            EndObject, FixMinHeight,
                        EndMember,
                        StartMember,
                            GO_Pages = PageObject, FRM_Type, FRTYPE_TAB_ABOVE,
                            PageMember,
                                VGroupObject, NormalOffset, NormalSpacing,
                                    StartMember,
                                        InfoObject,
                                            INFO_TextFormat,      getString( ABT_TEXT ),
                                            INFO_FixTextWidth, TRUE,
                                            INFO_MinLines,        4,
                                            FRM_Type,             FRTYPE_NONE,
                                        EndObject, FixMinHeight,
                                    EndMember,
                                    StartMember,
                                        HGroupObject, Spacing( 0 ), VOffset( 0 ), HOffset( 0 ),
                                            StartMember,
                                                InfoObject,
                                                    INFO_TextFormat,   getString( ABT_TEXT2A ),
                                                    INFO_FixTextWidth, TRUE,
                                                    INFO_MinLines,        1,
                                                    FRM_Type,            FRTYPE_NONE,
                                                EndObject,
                                            EndMember,
                                            StartMember,
                                                PrefButton( getString( ABT_TEXT2B ), ID_LICENSE ), FixMinWidth,
                                            EndMember,
                                            StartMember,
                                                InfoObject,
                                                    INFO_TextFormat, getString( ABT_TEXT2C ),
                                                    INFO_FixTextWidth, TRUE,
                                                    INFO_MinLines, 1,
                                                    FRM_Type, FRTYPE_NONE,
                                                EndObject,
                                            EndMember,
                                        EndObject, FixMinHeight,
                                    EndMember,
                                EndObject,

                                PageMember,
                                    VGroupObject, NormalOffset, NormalSpacing,
                                        StartMember,
                                            InfoObject,
                                                INFO_TextFormat,      getString( ABT_DEV_LABEL ),
                                                INFO_FixTextWidth, TRUE,
                                                INFO_HorizOffset,     0,
                                                INFO_VertOffset,      4,
                                                INFO_MinLines,        1,
                                                FRM_Type,             FRTYPE_NONE,
                                            EndObject, FixMinHeight,
                                        EndMember,
                                        StartMember,
                                            ListviewObject,
                                                LISTV_EntryArray,   developers,
                                                LISTV_ReadOnly,     TRUE,
                                                PGA_NewLook,        TRUE,
                                            EndObject,
                                        EndMember,
                                    EndObject,
                            EndObject,
                        EndMember,
                    EndObject,
                EndMember,
                StartMember,
                    PrefButton( getString( ABT_OK ), ID_OK ), FixMinHeight,
                EndMember,
            EndObject,
    EndObject;

    if( WO_About ) {
        AddMap( GO_Tabs, GO_Pages, tabs2pages );

        if( (win = WindowOpen( WO_About )) ) {
            GetAttr( WINDOW_SigMask, WO_About, &signal );

            do {
                Wait( signal );

                while( (rc = HandleEvent( WO_About )) != WMHI_NOMORE ) {
                    switch( rc ) {
                        case WMHI_CLOSEWINDOW:
                        case ID_OK:
                            running = FALSE;
                            break;

                        case ID_LICENSE:
                            /* TODO: Use OpenWorkbenchObjectA() instead, when its fully implemented. */
                            Execute( "Run >NIL: SYS:Utilities/More HELP:License", Input(), Output() );
                            break;
                    }
                }
            } while( running );
        }

        DisposeObject( WO_About );
    }
}

void ReadCredits( void ) {
    STRPTR buffer;
    LONG   size;

    if( (buffer = readFile( "HELP:Credits", &size )) ) {

    }
}


STRPTR readFile( STRPTR filename, LONG *size ) {
    struct FileInfoBlock fib;
    BPTR                 file;
    STRPTR               buffer;
    LONG                 bytesRead;

    if( !(file = Open( filename, MODE_OLDFILE )) ) return NULL;

    if( ExamineFH( file, &fib ) ) {
        *size = fib.fib_Size + 1;

        if( (buffer = AllocVec( *size, MEMF_ANY )) ) {
            bytesRead = Read( file, buffer, *size );

            if( bytesRead != (*size - 1) ) {
                FreeVec( buffer );
                buffer = NULL;
                *size  = 0;
            }
        }
    }

    Close( file );

    return buffer;
}