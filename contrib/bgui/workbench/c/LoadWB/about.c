#include "common.h"
#include "strings.h"
#include "developers.h"

/*** Prototypes *************************************************************/

SAVEDS VOID AboutTask( void );

/*** Global variables *******************************************************/

static ULONG AboutCount = 0;

/*** Local variables ********************************************************/

const enum
{
    ID_OK,
    ID_TABS
};

/*** Public functions *******************************************************/

SAVEDS VOID AboutTask( void )
{
    Object        *WO_About, *GO_Tabs, *GO_Pages;
    struct Window *win;
    ULONG          rc, signal = 0;
    BOOL           running    = TRUE;

    char          *pagelabels[] = { "About", "Developers" };
    ULONG          tabs2pages[] = { MX_Active, PAGE_Active, TAG_END };

    AboutCount++;

    WO_About = WindowObject,
	WINDOW_SmartRefresh, TRUE,
	WINDOW_AutoAspect,   TRUE,
	WINDOW_CloseOnEsc,   TRUE,
	WINDOW_RMBTrap,      TRUE,
	WINDOW_CloseGadget, FALSE,
	WINDOW_ScaleWidth,     15,
	WINDOW_ScaleHeight,    20,
	WINDOW_Title,           getString( ABT_WINDOWTITLE ),

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
					    EndObject,
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
		StartMember, PrefButton( getString( ABT_OK ), ID_OK ), FixMinHeight, EndMember,
	    EndObject,
    EndObject;

    if( WO_About )
    {
	AddMap( GO_Tabs, GO_Pages, tabs2pages );

	if( win = WindowOpen( WO_About ) )
	{
	    GetAttr( WINDOW_SigMask, WO_About, &signal );
	    do
	    {
		Wait( signal );
		while( (rc = HandleEvent( WO_About )) != WMHI_NOMORE )
		{
		    switch( rc )
		    {
			case WMHI_CLOSEWINDOW:
			case ID_OK:
			    running = FALSE;
			    break;
		    }
		}
	    } while( running );
	}

	DisposeObject( WO_About );
    }

    AboutCount--;
}
