#include "common.h"
#include "strings.h"

#define CMD_SIZE 1024
#define CON_FILE "CON:////AROS Shell/CLOSE/AUTO"

/*** Prototypes *************************************************************/

SAVEDS VOID ExecuteTask( void );

/*** Global variables *******************************************************/

/*** Local variables ********************************************************/

const enum {
    ID_STRING,
    ID_EXECUTE,
    ID_CANCEL
};

static char lastcommand[CMD_SIZE];
static BOOL lastcommand_init = TRUE;

/*** Public functions *******************************************************/

SAVEDS VOID ExecuteTask( void )
{
    Object        *WO_Execute, *GO_String;
    struct Window *win        = NULL;
    ULONG          rc, signal = 0;
    BOOL           running    = TRUE, runcommand = FALSE;
    BPTR           file;

    char          *tmp;

    /* Do we have to set up the lastcommand string? */
    if( lastcommand_init ) {
        strcpy( lastcommand, "" );
        lastcommand_init = FALSE;
    }

    WO_Execute = WindowObject,
        WINDOW_SmartRefresh, TRUE,
        WINDOW_AutoAspect,   TRUE,
        WINDOW_CloseOnEsc,   TRUE,
        WINDOW_RMBTrap,      TRUE,
        WINDOW_LockHeight,   TRUE,
        WINDOW_ScaleWidth,     20,
        WINDOW_Title,              getString( EXC_WINDOWTITLE ),

        WINDOW_MasterGroup,
            VGroupObject, NormalOffset, NormalSpacing,
                GROUP_BackFill, SHINE_RASTER,
                StartMember,
                    InfoObject,
                        INFO_TextFormat, getString( EXC_INSTRUCTIONS ),
                        DefaultFrame,
                    EndObject,
                EndMember,
                StartMember,
                    HGroupObject,
                        /* FIXME: How do I make the string-gadget active when the window opens? */
                        StartMember, GO_String = PrefString( NULL, lastcommand, CMD_SIZE, ID_STRING ), EndMember,
                        StartMember, ButtonObject, VIT_BuiltIn, BUILTIN_GETFILE, EndObject, FixMinWidth, EndMember,
                    EndObject,
                EndMember,
                StartMember,
                    SeparatorObject,
                        SEP_Horiz,    TRUE,
                        SEP_Recessed, TRUE,
                    EndObject, FixMinHeight,
                EndMember,
                StartMember,
                    HGroupObject, NormalSpacing, NormalSpacing,
                        StartMember, PrefButton( getString( EXC_EXECUTE ), ID_EXECUTE ), EndMember,
                        VarSpace( DEFAULT_WEIGHT ),
                        StartMember, PrefButton( getString( EXC_CANCEL ),  ID_CANCEL ), EndMember,
                    EndObject,
                EndMember,
            EndObject,
    EndObject;

    if( WO_Execute ) {
        if( (win = WindowOpen( WO_Execute )) ) {
            GetAttr( WINDOW_SigMask, WO_Execute, &signal );

            do {
                Wait( signal );

                while( (rc = HandleEvent( WO_Execute )) != WMHI_NOMORE ) {
                    switch( rc ) {
                        case WMHI_CLOSEWINDOW:
                        case ID_CANCEL:
                            running = FALSE;
                            break;

                        case ID_EXECUTE:
                        case ID_STRING:
                            GetAttr( STRINGA_TextVal, GO_String, (ULONG *) &tmp );
                            if( strlen( tmp ) > 0 ) {
                                strcpy( lastcommand, tmp );
                                runcommand = TRUE;
                            }
                            running = FALSE;
                            break;
                    }
                }
            } while( running );
        }

        DisposeObject( WO_Execute );
    }

    /* Run the command? */
    if( runcommand ) {
        if( (file = Open( CON_FILE, MODE_OLDFILE )) ) {
            SystemTags( lastcommand, SYS_Input, file, SYS_Output, NULL, TAG_END );
        }
    }
}
