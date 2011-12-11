#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>
#endif

#include "filexstrings.h"
#include "filexstructs.h"
#include "allprotos.h"

#define AREXXSTARTMENU 99

static ULONG displaymenuids[4] = {
	SHIFTMENU(3)+SHIFTITEM(2)+SHIFTSUB(1),
	SHIFTMENU(3)+SHIFTITEM(2)+SHIFTSUB(0),
	SHIFTMENU(3)+SHIFTITEM(2)+SHIFTSUB(2),
	SHIFTMENU(3)+SHIFTITEM(2)+SHIFTSUB(4)	};

/* Vergebene Shortcuts:
 * System:
 * N: New
 * O: Open
 * S: Save
 * A: Save as
 * P: Print
 * Q: Quit
 * X: Cut
 * C: Copy
 * V: Paste
 * Z: Undo
 * F: Search
 *
 * Eigene:
 * 
 * ?: About
 * I: Insert
 * M: Mark Block
 * T: Redo
 * G: Grab memory
 * J: Jump to byte
 * R: Replace
 * D: Append
 * K: Mark location
 * L: Jump to location
 * U: Miscellaneous
 * W: Adjust Windowsize
 * Y: Iconify
 * H: Commandshell
 * ^: Fill
 */

struct Menu *FileXMenus;

static struct NewMenu NewMenus[] = {
  	{NM_TITLE,   MSG_MENU_PROJECT, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_NEW, 0, 0, 0L, NULL},
/*		{NM_ITEM, "Test", "Shit", NM_COMMANDSTRING, 0L, (APTR)1L,*/
		{NM_ITEM, MSG_MENU_PROJECT_OPEN, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_OPENPREVIOUSLOADED, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_SAVE, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_SAVEAS, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_VIEW, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_NEW, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_SPLIT, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_NEXT, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_PREVIOUS, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_EXPAND, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_GROW, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_SHRINK, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_SAVE_AND_CLOSE, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_VIEW_CLOSE, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_WINDOW, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_WINDOW_NEW, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_WINDOW_SPLIT, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_WINDOW_NEXT, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_WINDOW_PREVIOUS, NULL, 0, 0L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_WINDOW_CLOSE, NULL, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_GRABMEMORY, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_PRINT, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_ICONIFY, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_ABOUT, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_PROJECT_QUIT, 0, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_EDIT, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_MARKBLOCK, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_CUT, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_COPY, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_FILL, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_PASTE, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_INSERT, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_APPEND, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_PRINT, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_LOADBLOCK, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_SAVEBLOCK, NULL, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_UNDO, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_REDO, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_EDIT_SWITCH, 0, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_SEARCH, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SEARCH_SEARCH, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SEARCH_REPLACE, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SEARCH_REPLACENEXT, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SEARCH_REPLACEPREVIOUS, 0, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_SETTING, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_ADJUSTWINDOW, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_DISPLAY, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_DISPLAY_CHARACTERS, NULL, CHECKIT|MENUTOGGLE, 22L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_DISPLAY_ASCII,NULL, CHECKIT|CHECKED|MENUTOGGLE, 21L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_DISPLAY_ALL,NULL, CHECKIT|MENUTOGGLE, 19L, NULL},
			{NM_SUB, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_DISPLAY_USERDEFINED, NULL, CHECKIT|MENUTOGGLE, 7L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_DISPLAY_LOAD, NULL, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_FONT, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_SCREEN, NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_SCREEN_DEFAULTPUBLIC,NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_SCREEN_WORKBENCH,NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_SCREEN_PUBLIC,NULL, 0, 0L, NULL},
			{NM_SUB, MSG_MENU_SETTINGS_SCREEN_OWNPUBLIC,NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_PALETTE, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_MISCELLANEOUS,0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_LOADSETTINGS, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_SAVESETTINGS, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_SETTINGS_SAVESETTINGSAS, NULL, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_MOVE, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_MOVE_JUMPTOBYTE, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_MOVE_MARKLOCATION, 0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_MOVE_JUMPTOMARK, 0, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_AREXX, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_COMMANDSHELL, 0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_EXECUTECOMMAND,0, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_ASSIGNCOMMAND,0, 0, 0L, NULL},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, 0, (STRPTR)"0", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"1", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"2", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"3", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"4", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"5", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"6", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"7", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"8", 0, 0L, (APTR)1L},
		{NM_ITEM, 0, (STRPTR)"9", 0, 0L, (APTR)1L},
		{NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_LOADCOMMANDS, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_SAVECOMMANDS,NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_AREXX_SAVECOMMANDSAS,NULL, 0, 0L, NULL},
	{NM_TITLE, MSG_MENU_MISC, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_MISC_OPENCALC, NULL, 0, 0L, NULL},
		{NM_ITEM, MSG_MENU_MISC_OPENCLIPCONV, NULL, 0, 0L, NULL},
	{NM_END, NULL, NULL, 0, 0L, NULL} };

/*
 * void SetEmptyARexxMenu( WORD Number )
 * 
 * Setzt ein ARexxMenu auf "Empty Command #x".
 */

void SetEmptyARexxMenu( WORD Number )
{
	CONST_STRPTR Str = GetStr( MSG_MENU_AREXX_EMPTYCOMMANDX );

	if( arexxcommands[ Number ] )
		FreeVec( arexxcommands[ Number ] );

	if(( arexxcommands[ Number ] = AllocVec( strlen( Str ) + 1 ,MEMF_ANY )))
	{
		sprintf( arexxcommands[ Number ], Str, (WORD) Number );

		NewMenus[ AREXXSTARTMENU + Number ].nm_Label = arexxcommands[ Number ];

		arexxcommandused[ Number ] = FALSE;
	}
}


/*
 * void SetARexxMenu( WORD Number, UBYTE *Name )
 * 
 * Setzt ein ARexxMenu auf eine bestimmte Zeichenkette.
 */

void SetARexxMenu( WORD Number, UBYTE *Name )
{
	if( arexxcommands[ Number ] )
		FreeVec( arexxcommands[ Number ] );

	if(( arexxcommands[ Number ] = AllocVec( strlen( Name ) + 1, MEMF_ANY )))
	{
		strcpy( arexxcommands[ Number ], Name );

		arexxcommandused[ Number ] = TRUE;

		NewMenus[ AREXXSTARTMENU + Number ].nm_Label = FilePart( arexxcommands[ Number ]);
	}
}


/*
 * void LocalizeMenus( void )
 * 
 * Lokalisiert die Menus
 */

void LocalizeMenus( void )
{
	struct NewMenu *NewMenu = NewMenus;
	CONST_STRPTR Shortcut, Label;

	while( NewMenu->nm_Type != NM_END )
	{
		if(( NewMenu->nm_Label != NM_BARLABEL ) && ( !NewMenu->nm_UserData ))
		{
			Shortcut = GetStr( NewMenu->nm_Label );

			if(Shortcut[0] && !Shortcut[1])
				Label = Shortcut + 2;
			else
			{
				Label		= Shortcut;
				Shortcut	= NULL;
			}

			switch(NewMenu->nm_Type)
			{
				case NM_ITEM:
				case NM_SUB:
					NewMenu->nm_CommKey = Shortcut;
					break;
			}

			NewMenu->nm_Label = Label;
		}

		NewMenu++;
	}
}


/*
 * void MyFreeMenus( void )
 * 
 * Entfernt die Menus von Fenster und gibt sie frei.
 */

void MyFreeMenus( void )
{
	if(FileXMenus)
	{
		struct DisplayInhalt *DI;

		DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

		while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
		{
			if( DI->Wnd )
				ClearMenuStrip(DI->Wnd);

			DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
		}


		FreeMenus(FileXMenus);
		FileXMenus = NULL;
	}
}


/*
 * void SetNewMenus( void )
 * 
 * Gibt die Menus frei, erzeugt sie neu und setzt sie.
 */

void SetNewMenus( void )
{
	MyFreeMenus();

	if((FileXMenus = CreateMenus(NewMenus, GTMN_FrontPen, 0L, TAG_DONE)))
	{
		struct DisplayInhalt *DI;

		LayoutMenus(FileXMenus, VisualInfo,GTMN_NewLookMenus,TRUE,TAG_DONE);

		DI = (struct DisplayInhalt *)DisplayInhaltList.lh_Head;

		while( DI != ( struct DisplayInhalt * )&DisplayInhaltList.lh_Tail )
		{
			if( DI->Wnd )
				SetMenuStrip(DI->Wnd, FileXMenus);

			DI = ( struct DisplayInhalt *)DI->Node.ln_Succ;
		}

		SetAllCheckMarks();
	}
}

static void SetCheckMark( ULONG MenuID, BOOL status )
{
	struct MenuItem *mi;

	if( FileXMenus )
	{
		mi = ItemAddress( FileXMenus, MenuID );

		if( status )
			mi->Flags |= CHECKED;
		else
			mi->Flags &= ~CHECKED;
	}
}

void SetDisplayCheckMarks( void )
{
	int k;

	for( k = 0; k < 4; k++ )
		if( AktuDD->DisplayTyp == k )
			SetCheckMark( displaymenuids[ k ], TRUE );
		else
			SetCheckMark( displaymenuids[ k ], FALSE );
}

void SetAllCheckMarks( void )
{
	SetDisplayCheckMarks();
}

