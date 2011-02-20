/*
 * @(#) $Header$
 *
 * PopButtonDemo.c
 *
 * (C) Copyright 1999 BGUI Developement team.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 *	NMC (12.12.95): Added another menu to demonstrate enabling
 *			and disabling.
 *
 * $Log$
 * Revision 42.2  2003/01/18 19:10:20  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.1  2000/07/11 17:07:40  stegerg
 * Did not open intuition.library (SAS auto lib opening?)
 *
 * Revision 42.0  2000/05/09 22:21:29  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:15  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:15  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/05/31 01:42:39  mlemos
 * Ian sources.
 *
 *
 *
 */

#include <exec/types.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/bgui.h>
#include <proto/dos.h>
#include <proto/utility.h>

/*
 *	Object ID.
 */
#define ID_QUIT                 1
#define ID_POPMENU1		2
#define ID_POPMENU2		3
#define ID_POPMENU3		4
#define ID_POPMENU4		5	/* NMC */

/*
 *	Menu entries.
 */
struct PopMenu Project[] = {
	{ "New",          0, 0 },
	{ "Open...",      0, 0 },
	{ PMB_BARLABEL,	0, 0 },
	{ "Save",         0, 0 },
	{ "Save As...",   0, 0 },
	{ PMB_BARLABEL,	0, 0 },
	{ "Print",        0, 0 },
	{ "Print As...",  0, 0 },
	{ PMB_BARLABEL,	0, 0 },
	{ "About...",     0, 0 },
	{ PMB_BARLABEL,	0, 0 },
	{ "Quit",         0, 0 },
	{ NULL,		0, 0 } };

struct PopMenu	Edit[] = {
	{ "Cut",          0, 0 },
	{ "Copy",         0, 0 },
	{ "Paste",        0, 0 },
	{ PMB_BARLABEL,	0, 0 },
	{ "Erase",        0, 0 },
	{ NULL,		0, 0 } };

/*
 *	This menu has checkable items and mutual exclusion.
 *
 *	The first item will mutually-exclude the last
 *	four items and any of the last four items will
 *	mutually-exclude the first item.
 */
struct PopMenu	Exclude[] = {
    { "Uncheck below",        PMF_CHECKIT,                    (1<<2)|(1<<3)|(1<<4)|(1<<5), },
    { PMB_BARLABEL,		0,				0, },
    { "Item 1",               PMF_CHECKIT|PMF_CHECKED,        (1<<0), },
    { "Item 2",               PMF_CHECKIT|PMF_CHECKED,        (1<<0), },
    { "Item 3",               PMF_CHECKIT|PMF_CHECKED,        (1<<0), },
    { "Item 4",               PMF_CHECKIT|PMF_CHECKED,        (1<<0), },
    { NULL,			0,				0 },
};

/*
 *	This menu has two items that enable the other
 *	when selected. (NMC)
 */
struct PopMenu	Able[] = {
   {	"Enable below",		0,		0, },
   {	"Enable above",		PMF_DISABLED,	0, },
   {	NULL,			0,		0, },
};

/*
 *	Library base and class base.
 */
struct Library *BGUIBase;
#ifdef __AROS__
struct IntuitionBase *IntuitionBase;
#endif

/*
 *	Put up a simple requester.
 */
ULONG ReqA( struct Window *win, UBYTE *gadgets, UBYTE *body, IPTR *args )
{
	struct bguiRequest	req = { NULL };

	req.br_GadgetFormat	= gadgets;
	req.br_TextFormat	= body;
	req.br_Flags		= BREQF_CENTERWINDOW|BREQF_AUTO_ASPECT|BREQF_LOCKWINDOW|BREQF_FAST_KEYS;

	return BGUI_RequestA( win, &req, args);
}
#define Req(win, gadgets, body, ...) \
({ IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
   ReqA(win, gadgets, body, __args); })

int main( int argc, char **argv )
{
	struct Window		*window;
	Object			*WO_Window, *GO_Quit, *GO_PMB, *GO_PMB1, *GO_PMB2, *GO_PMB3; /* NMC */
	ULONG			 signal, rc, tmp = 0;
	UBYTE			*txt;
	BOOL			 running = TRUE;

	/*
	 *	Open BGUI.
	 */
#ifdef __AROS__
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39);
	if (IntuitionBase)
#endif
	if (( BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION ))) {
			/*
			 *	Create the popmenu buttons.
			 */
			GO_PMB	= BGUI_NewObject(BGUI_POPBUTTON_GADGET,
							     PMB_MenuEntries,	 Project,
							     /*
							      *         Let this one activate
							      *         the About item.
							      */
							     PMB_PopPosition,	 9,
							     LAB_Label,          "_Project",
							     LAB_Underscore,	 '_',
							     GA_ID,		 ID_POPMENU1,
							     TAG_END );

			GO_PMB1 = BGUI_NewObject(BGUI_POPBUTTON_GADGET,
							     PMB_MenuEntries,	 Edit,
							     LAB_Label,          "_Edit",
							     LAB_Underscore,	 '_',
							     GA_ID,		 ID_POPMENU2,
							     TAG_END );

			GO_PMB2 = BGUI_NewObject(BGUI_POPBUTTON_GADGET,
							     PMB_MenuEntries,	 Exclude,
							     LAB_Label,          "E_xclude",
							     LAB_Underscore,	 '_',
							     GA_ID,		 ID_POPMENU3,
							     TAG_END );

			GO_PMB3 = BGUI_NewObject(BGUI_POPBUTTON_GADGET,
							     PMB_MenuEntries,	 Able,	/* NMC */
							     /*
							      *         Make this menu always
							      *         appear below the label
							      */
							     PMB_PopPosition,	 ~0,
							     LAB_Label,          "E_nable",
							     LAB_Underscore,	 '_',
							     GA_ID,		 ID_POPMENU4,
							     TAG_END );
			/*
			 *	Create the window object.
			 */
			WO_Window = WindowObject,
				WINDOW_Title,		"PopButtonClass Demo",
				WINDOW_AutoAspect,	TRUE,
				WINDOW_SmartRefresh,	TRUE,
				WINDOW_RMBTrap,         TRUE,
				WINDOW_MasterGroup,
					VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
						GROUP_BackFill,         SHINE_RASTER,
						StartMember,
							HGroupObject, Spacing( 4 ), HOffset( 6 ), VOffset( 4 ),
								NeXTFrame,
								FRM_BackDriPen,         FILLPEN,
								StartMember, GO_PMB, FixMinWidth, EndMember,
								StartMember, VertSeparator, EndMember,
								StartMember, GO_PMB1, FixMinWidth, EndMember,
								StartMember, VertSeparator, EndMember,
								StartMember, GO_PMB2, FixMinWidth, EndMember,
								StartMember, VertSeparator, EndMember,
								StartMember, GO_PMB3, FixMinWidth, EndMember,	/* NMC */
								StartMember, VertSeparator, EndMember,		/* NMC */
							EndObject, FixMinHeight,
						EndMember,
						StartMember,
							InfoFixed( NULL, ISEQ_C
									 "This demonstrates the usage of the \"PopButtonClass\"\n"
									 "When you click inside the above popmenu buttons a small\n"
									 "popup-menu will appear which you can choose from.\n\n"
									 "You can also key-activate the menus and browse though the\n"
									 "items using the cursor up and down keys. Return or Enter\n"
									 "acknowledges the selection and escape cancels it.",
									 NULL, 7 ),
						EndMember,
						StartMember,
							HGroupObject,
								VarSpace( DEFAULT_WEIGHT ),
								StartMember, GO_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
								VarSpace( DEFAULT_WEIGHT ),
							EndObject, FixMinHeight,
						EndMember,
					EndObject,
			EndObject;

			/*
			 *	Object created OK?
			 */
			if ( WO_Window ) {
				/*
				** NMC: Amended to not rely on return from
				** GadgetKey == 1 exactly. Yes, Jan, I know
				** you know it will be 1 - you wrote it! :-)
				**/
				if (GadgetKey( WO_Window, GO_Quit,  "q" ) &&
				    GadgetKey( WO_Window, GO_PMB,   "p" ) &&
				    GadgetKey( WO_Window, GO_PMB1,  "e" ) &&
				    GadgetKey( WO_Window, GO_PMB2,  "x" ) &&
				    GadgetKey( WO_Window, GO_PMB3,  "n" )
				    ) {
					if (( window = WindowOpen( WO_Window ))) {
						GetAttr( WINDOW_SigMask, WO_Window, &signal );
						do {
							Wait( signal );
							while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
								switch ( rc ) {

									case	WMHI_CLOSEWINDOW:
									case	ID_QUIT:
										running = FALSE;
										break;

									case	ID_POPMENU4:
										GetAttr( PMB_MenuNumber, GO_PMB3, &tmp );
										DoMethod( GO_PMB3, PMBM_ENABLE_ITEM, tmp ^ 1 );
										DoMethod( GO_PMB3, PMBM_DISABLE_ITEM, tmp );
										txt = Able[ tmp ].pm_Label;
										goto def;

									case	ID_POPMENU3:
										GetAttr( PMB_MenuNumber, GO_PMB2, &tmp );
										txt = Exclude[ tmp ].pm_Label;
										goto def;

									case	ID_POPMENU2:
										GetAttr( PMB_MenuNumber, GO_PMB1, &tmp );
										txt = Edit[ tmp ].pm_Label;
										goto def;

									case	ID_POPMENU1:
										GetAttr( PMB_MenuNumber, GO_PMB, &tmp );
										switch ( tmp ) {
											case	9:
												Req( window, "*OK", ISEQ_C ISEQ_B "PopButtonClass DEMO\n" ISEQ_N "(C) Copyright 1995 Jaba Development." );
												break;

											case	11:
												running = FALSE;
												break;

											default:
												txt = Project[ tmp ].pm_Label;
												def:
												Req( window, "*OK", ISEQ_C "Selected Item %ld <" ISEQ_B "%s" ISEQ_N ">", tmp, txt );
												break;
										}
										break;
								}
							}
						} while ( running );
					}
				}
				DisposeObject( WO_Window );
		}
		CloseLibrary( BGUIBase );
	}

	return 0;
}

#ifdef _DCC
int wbmain( struct WBStartup *wbs )
{
	return( main( 0, wbs ));
}
#endif

