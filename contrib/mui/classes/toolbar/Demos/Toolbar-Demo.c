/*
** Example code for Toolbar.mcc.
** $VER: Toolbar-Demo.c V1.1 (26-Feb-00)
**
** Any questions or requests can be written to
** Floyd@Amiga.DK
**
** Find the latest version of Toolbar.mcc at my homepage
** http://www.diku.dk/students/benny/
*/

/* ANSI C */
//#include <stdlib.h>
#include <stdio.h> /* exit() */

/* MUI */
#include <libraries/mui.h>
#include <proto/muimaster.h>

/* Protos */
#include <proto/exec.h> /* OpenLibrary */

#ifdef __GNUC__
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/muimaster_protos.h>
#endif
#ifdef __SASC
#include <clib/alib_protos.h> /* DoMethod */
#endif

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#if !defined(__AROS__)
#define REG(x) register __ ## x
#endif

enum {NEW=0, OPEN, CUT=3, COPY, PASTE, UNDO };
enum {BOLD=0, ITALIC, UNDERLINED, LEFT=4, CENTER, RIGHT };

#include <MUI/Toolbar_mcc.h>

#if !defined(__AROS__)
struct Library *MUIMasterBase;

VOID TestFunc(REG(a0) struct Hook *hook, REG(a1) ULONG *prms)
#else
AROS_UFH3(void, TestFunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(void *, unused, A2),
    AROS_UFHA(ULONG *, prms, A1))
#endif
{
#if defined(__AROS__)
	AROS_USERFUNC_INIT
#endif

	ULONG qualifier = prms[0];

	printf("Qualifier: %x\n", qualifier);

#if defined(__AROS__)
	AROS_USERFUNC_EXIT
#endif
}

const struct Hook TestHook  = { { NULL,NULL }, (HOOKFUNC)TestFunc,NULL,NULL };

int main(int argc,char *argv[])
{
	Object 	*app;
	Object 	*WI_Main;
	Object	*TB_Bank1;
	Object	*TB_Bank2;
	Object	*TX_Status;
	Object	*TX_About;

	ULONG		sigs = 0;
	ULONG		res = 5;

	/* Here the contents of the toolbar is described.
           Make the description-array and you are almost finished.
           Notice that bank1 makes use of the supplied Macro-definitions
           in mui/Toolbar_mcc.h */

	struct MUIP_Toolbar_Description bank1[] =
	{
		Toolbar_KeyButton(0, "New",   'n'),
		Toolbar_KeyButton(TDF_GHOSTED, "Open",  'o'),
		Toolbar_Space,
		Toolbar_KeyButton(0, "Cut",   'c'),
		Toolbar_KeyButton(0, "Copy",  'y'),
		Toolbar_KeyButton(0, "Paste", 'p'),
		Toolbar_KeyButton(0, "Undo",  'u'),
		Toolbar_End
	};

	struct MUIP_Toolbar_Description bank2[] =
	{
		{ TDT_BUTTON, 0, TDF_TOGGLE | TDF_SELECTED, 0, "Bold text", 0},
		{ TDT_BUTTON, 0, TDF_TOGGLE, 0, "Italic text", 0},
		{ TDT_BUTTON, 0, TDF_TOGGLE, 0, "Underlined text", 0},
		{ TDT_SPACE, NULL, NULL, NULL, NULL, NULL },
	/* Notice the mutual-exclude flag in the three following buttons. The bit pattern correspond
		to the buttons/fields which should be deactivated. */
		{ TDT_BUTTON, 0, TDF_RADIOTOGGLE | TDF_SELECTED, 0, "Left aligned", 0x0020 | 0x0040},
		{ TDT_BUTTON, 0, TDF_RADIOTOGGLE, 0, "Centered", 0x0010 | 0x0040},
		{ TDT_BUTTON, 0, TDF_RADIOTOGGLE, 0, "Right aligned", 0x0010 | 0x0020},
		{ TDT_END, 0, 0, 0, 0, 0 }
	};

	/* Let us make sure we have enough stack-space - we'll ad 4Kb to the stack */
#if !defined(__AROS__)
	struct StackSwapStruct stackswap;
	struct Task *mytask   = FindTask(NULL);
	ULONG  stacksize      = (ULONG)mytask->tc_SPUpper-(ULONG)mytask->tc_SPLower+4096;
	APTR   newstack       = AllocVec(stacksize, 0L);

	stackswap.stk_Lower   = newstack;
	stackswap.stk_Upper   = (ULONG)newstack+stacksize;
	stackswap.stk_Pointer = (APTR)stackswap.stk_Upper;

	if(newstack)
	{
		StackSwap(&stackswap);
		if (MUIMasterBase = OpenLibrary(MUIMASTER_NAME, 19))
		{
#endif
			app = ApplicationObject,
				MUIA_Application_Title      , "Toolbar-Demo",
				MUIA_Application_Version    , "$VER: Toolbar-Demo V1.1 (26-Feb-00)",
				MUIA_Application_Copyright  , "©1997-00 Benny Kjær Nielsen",
				MUIA_Application_Author     , "Benny Kjær Nielsen",
				MUIA_Application_Description, "Toolbar.mcc demo program",
				MUIA_Application_Base       , "TOOLBAR",
				SubWindow, WI_Main = WindowObject,
					MUIA_Window_Title, "Toolbar Demo",
					MUIA_Window_ID   , MAKE_ID('M','A','I','N'),
					WindowContents, HGroup,
						Child, VGroup,
							Child, HGroup,
								Child, RectangleObject, End,
								Child, TB_Bank1  = ToolbarObject,
								/* Always specify all the filenames. A user might want to make some
									nice (perhaps animated) graphics for the select-image or he might
									want to make his own ghost-effect.
									If a file doesn't exist then the class simply uses the normal image
									and adds the effect which is specified by the user in the preferences.
									No images have to exist since the toolbar then just changes to textmode..
									Remember to specify the filenames in your documentation.*/

									MUIA_Toolbar_ImageType, 	MUIV_Toolbar_ImageType_File,
									MUIA_Toolbar_ImageNormal,	"PROGDIR:Images/ButtonBank1.bsh",
									MUIA_Toolbar_ImageSelect,	"PROGDIR:Images/ButtonBank1s.bsh",
									MUIA_Toolbar_ImageGhost,	"PROGDIR:Images/ButtonBank1g.bsh",
									MUIA_Toolbar_Description,	bank1,

									/* Default font - this is only used if the user has *not* selected
									   a fonttype in the toolbar preferences */
									MUIA_Font,						MUIV_Font_Tiny,

									MUIA_ShortHelp, FALSE, /* Enable/disable bubblehelp */
									MUIA_Draggable, TRUE,
								End,
								Child, RectangleObject, End,
							End,
							Child, TX_Status = MUI_NewObject("InfoText.mcc",
								/*	MUIA_Text_SetVMax,	FALSE, */
							End,
							Child, TX_About = FloattextObject,
								MUIA_Background, MUII_TextBack,
								TextFrame,
								MUIA_Floattext_Text,	"This is a simple demonstration of the features "
															"offered by the Toolbar class. If you want to know "
															"all the details then just take a quick look in "
															"the autodoc.\n\n"
															"Comments, suggestions, bugreports, etc. can be "
															"e-mailed to Floyd@Amiga.DK or Benny@DIKU.DK.\n\n"
															"Latest release is available at:\n"
															"  http://www.diku.dk/students/benny",
							End,
						End,
						Child, VGroup,
							Child, RectangleObject, End,
							Child, TB_Bank2  = ToolbarObject,
								MUIA_Toolbar_ImageType, 	MUIV_Toolbar_ImageType_File,
								/* Use MUIA_Toolbar_Path to simplify things and ease the configurability.
								   You only need to change this attribute to change toolbar images */
								MUIA_Toolbar_Path,			"PROGDIR:Images/",
								MUIA_Toolbar_ImageNormal,	"ButtonBank2.bsh",
								MUIA_Toolbar_ImageSelect,	"ButtonBank2s.bsh",
								MUIA_Toolbar_ImageGhost,	"ButtonBank2g.bsh",
								MUIA_Toolbar_Description,	bank2,
								MUIA_ShortHelp, TRUE, /* Enable/disable bubblehelp */
								MUIA_Toolbar_Horizontal, FALSE,
							End,
							Child, RectangleObject, End,
						End,
					End,
				End,
			End;

			if (app)
			{
				ULONG open;

				DoMethod(WI_Main,	MUIM_Notify,	MUIA_Window_CloseRequest,	TRUE ,
							app,	2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

				/* An example of a notification event */
				DoMethod(TB_Bank1, MUIM_Toolbar_Notify, NEW, MUIV_Toolbar_Notify_Pressed, FALSE,
							app,	2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

				/* And naturally you can kill the notification event if you want, but we don't
					DoMethod(TB_Bank1, MUIM_Toolbar_KillNotify, 0, MUIV_Toolbar_Notify_Pressed);*/

				/* Another notification event to show the use of MUIM_Toolbar_Set.
					This could just as well have been done with the mutual-exclude flags */
				DoMethod(TB_Bank2, MUIM_Toolbar_Notify, UNDERLINED, MUIV_Toolbar_Notify_Pressed, FALSE,
							MUIV_Notify_Self, 4, MUIM_Toolbar_Set, ITALIC, MUIV_Toolbar_Set_Selected, FALSE);

				/* A bad example of MUIV_EveryTime/MUIV_TriggerValue, but it illustrates
				   the possiblities in these special values.  */
				DoMethod(TB_Bank1, MUIM_Toolbar_Notify, COPY, MUIV_Toolbar_Notify_Pressed, MUIV_EveryTime,
							TB_Bank2,	4, MUIM_Toolbar_Set, BOLD, MUIV_Toolbar_Set_Selected, MUIV_TriggerValue);

				/* You can connect the toolbar helpstrings with a text-object or anything
					that accepts a string pointer eg. the window/screen title */
/*				DoMethod(TB_Bank1, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
							TX_Status,	3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

				DoMethod(TB_Bank2, MUIM_Notify, MUIA_Toolbar_HelpString, MUIV_EveryTime,
							TX_Status,	3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);
*/
				DoMethod(TB_Bank1, MUIM_Toolbar_Notify, COPY, MUIV_Toolbar_Notify_Pressed, MUIV_EveryTime,
							MUIV_Notify_Application, 3, MUIM_CallHook, &TestHook, MUIV_Toolbar_Qualifier);

				DoMethod(TB_Bank1, MUIM_Toolbar_Notify, COPY, MUIV_Toolbar_Notify_Pressed, MUIV_EveryTime,
							TX_Status,	4, MUIM_SetAsString, MUIA_Text_Contents, "Qualifier: %lx", MUIV_Toolbar_Qualifier);

				set (WI_Main, MUIA_Window_Open, TRUE);
				get(WI_Main,MUIA_Window_Open,&open);

				if(open)
				{
					res = 0;
					while (DoMethod(app,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit)
					{
						if (sigs)
						{
							sigs = Wait(sigs | SIGBREAKF_CTRL_C);

							/* quit when receiving break from console */
							if (sigs & SIGBREAKF_CTRL_C)
								break;
						}
					}
				}
				MUI_DisposeObject(app);
			}
			CloseLibrary(MUIMasterBase);
#if !defined(__AROS__)
		}
		StackSwap(&stackswap);
		FreeVec(newstack);
	}
	exit(res);
#else
	return 0;
#endif
}
