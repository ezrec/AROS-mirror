/*
** $Id$
*/

#include "rev.h"
#include "mystring.h"
#include "textinput_mcc.h"
#include "textinput_cfg.h"
#include "cat.h"

#define UserLibID "$VER: Textinput.mcp " LVERTAG " © 1997-" COMPILEYEAR " Oliver Wagner <owagner@vapor.com>, All Rights Reserved\r\n"
#define CLASS "Textinput.mcp"
#define SUPERCLASSP MUIC_Mccprefs
#define _DispatcherP dispfunc
#define MASTERVERSION 18
static APTR prefsiconobj( void );
#define PREFSIMAGEOBJECT prefsiconobj()
#define ClassInit
#define ClassExit

#define NUMKEYS 7

struct DataP {
	APTR pp_pens[ 13 ];
	UBYTE grabcol[ 13 ];
	int grabcols;
	APTR str_font;
	APTR str_fixedfont;
	APTR pi_editbut;
	APTR cyc_cursorstyle;
	APTR nm_cursorsize;
	APTR nm_blinkrate;
	APTR nm_wordwrap;
	APTR chk_wordwrap;
	APTR chk_popup_single, chk_popup_multi, chk_mailquote;
	APTR nm_undobytessingle;
	APTR nm_undolevelssingle;
	APTR nm_undobytesmulti;
	APTR nm_undolevelsmulti;
	APTR str_editor;
	APTR lv_keys, bt_keyadd, bt_keydel, str_hotkey, po_action, txt_action, lv_action;
	APTR chk_sync;
	APTR t1, t2, t3;
};
#define Data DataP

#include "mccheader.h"

static struct MUI_CustomClass *numericclass;
static struct Catalog *catalog;
#if INCLUDE_VERSION < 44
struct Library *LocaleBase;
#else
struct LocaleBase *LocaleBase;
#endif

#define DEBUG kprintf
//#define DEBUG /##/

#define MLTEXT \
 "Textinput MUI Custom Class " LVERTAG "\n\n"\
 "This is a single or multi line text input class.\n"\
 "It can serve as a general replacement for the\n"\
 "old style user unfriendly BOOPSI string gadgets,\n"\
 "but also has `Editor'-alike multi line capabilities.\n"\
 "\nFor developer information, see ftp.vapor.com, /pub/misc."

//	sprintf() replacement
UWORD fmtfunc[] = { 0x16c0, 0x4e75 };
void __stdargs sprintf( char *to, char *fmt, ... )
{
	RawDoFmt( fmt, &fmt + 1, (APTR)fmtfunc, to );
}

static APTR __stdargs DoSuperNew( struct IClass *class, APTR obj, ULONG tag1, ... )
{
	return( (APTR)DoSuperMethod( class, obj, OM_NEW, &tag1, NULL ) );
}

static APTR hotkeystring( void )
{
	APTR o;

	o = PophotkeyObject, 
		MUIA_CycleChain, 1,
		MUIA_String_MaxLen, 80,
		MUIA_Weight, 200,
	End;

	if( !o )
		o = TextinputObject, StringFrame, MUIA_CycleChain, 1, MUIA_Weight, 200, End;

	return( o );
}

static STRPTR actions[] = {
	"Revert changes",
	"Del line",
	"Begin mark",
	"Mark all",
	"Cut",
	"Copy",
	"Paste",
	"External edit",
	"Toggle wordwrap",
	"Cursor left",
	"Cursor right",
	"Cursor up",
	"Cursor down",
	"Page up",
	"Page down",
	"Top",
	"Bottom",
	"Line start",
	"Line end",
	"Prev. word",
	"Next word",
	"Popup",
	"Delete",
	"Del EOL",
	"Backspace",
	"BS SOL",
	"(null)",
	"(null)",
	"Del Word",
	"BS Word",
	"Insert file",
	"Toggle case",
	"Toggle case EOW",
	"Increment decimal",
	"Decrement decimal",
	"Undo",
	"Redo",
	"Tab",
	"Next gadget",
	"Set bookmark 1",
	"Set bookmark 2",
	"Set bookmark 3",
	"Goto bookmark 1",
	"Goto bookmark 2",
	"Goto bookmark 3",
	"Cut line",
	"Copy & Cut",
	NULL
};

static int __asm __saveds displayfunc( register __a1 struct textinput_keydef *kd, register __a2 STRPTR *array )
{
	if( kd )
	{
		*array++ = kd->key;
		*array++ = "=";
		*array = actions[ kd->action ];
	}
	else
	{
		*array++ = GS( PREFS_L1 );
		*array++ = "";
		*array = GS( PREFS_L2 );
	}

	return( 0 );
}

static struct Hook disp_hook = { 0, 0, (HOOKFUNC)displayfunc };

static void __asm __saveds objstrfunc( register __a2 Object *pop,register __a1 Object *str )
{
	char *x;
	DoMethod( pop, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &x );
	set( str, MUIA_Text_Contents, x );
}

static const struct Hook objstr_hook = { 0, 0, (HOOKFUNC)objstrfunc };

static void __asm __saveds windowfunc( register __a2 Object *pop,register __a1 Object *win )
{
	set( win, MUIA_Window_DefaultObject, pop );
}

static const struct Hook window_hook = { 0, 0, (HOOKFUNC)windowfunc };

void __asm AsmFreePooled(register __a0 void *, register __a1 void *, register __d0 ULONG, register __a6 struct ExecBase *);
void * __asm AsmAllocPooled(register __a0 void *, register __d0 ULONG, register __a6 struct ExecBase *);

static APTR __asm __saveds constructfunc( register __a1 struct textinput_keydef *kd, register __a2 APTR pool )
{
	struct textinput_keydef *nkd;
	nkd = AsmAllocPooled( pool, sizeof( *nkd ), SysBase );
	if( nkd )
		*nkd = *kd;
	return( nkd );
}
static const struct Hook const_hook = { 0, 0, (HOOKFUNC)constructfunc };

static void __asm __saveds destructfunc( register __a1 struct textinput_keydef *kd, register __a2 APTR pool )
{
	AsmFreePooled( pool, kd, sizeof( *kd ), SysBase );
}
static struct Hook dest_hook = { 0, 0, (HOOKFUNC)destructfunc };

DECCONST
{
	struct Data *data;
	APTR grp;
	static STRPTR prefstitles[] = { "Style", "Keys", "Options", "External", "Sample", NULL };
	static STRPTR colourtitles[] = { "Standard", "Special", NULL };
	static STRPTR underlineopts[] = { "Dashed", "Dotted", "Solid", NULL };
	static STRPTR cursoropts[] = { "Block", "Vert Line", "Horiz Line", NULL };
	static STRPTR checkeropts[] = { "Ispell w/ ARexx", /*"AlphaSpell",*/ NULL };
	APTR bt_grabstringcols, bt_defaultkeys, bt_action;
	int c;

	obj = (APTR)DOSUPER;
	if( !obj )
		return( 0 );

	data = INST_DATA( cl, obj );

	grp = VGroup,

		//Child, MUI_MakeObject( MUIO_BarTitle, "Textinput · Advanced Text Input Class" ),

		Child, RegisterGroup( prefstitles ), MUIA_CycleChain, 1,

			Child, HGroup,

				Child, RegisterGroup( colourtitles ), MUIA_CycleChain, 1,

					Child, VGroup, GroupFrameT( GS( PREFS_COLORS ) ),

						Child, ColGroup( 3 ),

							Child, HSpace( 0 ),
							Child, MUI_MakeObject( MUIO_Label, GS( PREFS_FOREGROUND ), MUIO_Label_Centered ),
							Child, MUI_MakeObject( MUIO_Label, GS( PREFS_BACKGROUND ), MUIO_Label_Centered ),
							Child, Label2( GS( PREFS_INACTIVE ) ),
							Child, data->pp_pens[ 0 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m5",
									MUIA_Window_Title, "Inactive/Foreground",
							End,
							Child, data->pp_pens[ 1 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m2",
								MUIA_Window_Title, "Inactive/Background",
							End,

							Child, Label2( GS( PREFS_ACTIVE ) ),
							Child, data->pp_pens[ 2 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m5",
								MUIA_Window_Title, "Active/Foreground",
							End,
							Child, data->pp_pens[ 3 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m2",
								MUIA_Window_Title, "Active/Background",
							End,

							Child, Label2( GS( PREFS_MARKED ) ),
							Child, data->pp_pens[ 4 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m2",
								MUIA_Window_Title, "Marked/Foreground", 
							End,
							Child, data->pp_pens[ 5 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m5",
								MUIA_Window_Title, "Marked/Background",
							End,

							Child, Label2( GS( PREFS_CURSOR ) ),
							Child, data->pp_pens[ 6 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m7",
								MUIA_Window_Title, "Cursor/Foreground",
							End,
							Child, data->pp_pens[ 7 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m6",
								MUIA_Window_Title, "Cursor/Background",
							End,

							Child, Label2( GS( PREFS_STYLE ) ),
							Child, data->pp_pens[ 8 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m2",
								MUIA_Window_Title, "Style/Foreground",
							End,
							Child, data->pp_pens[ 9 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m5",
								MUIA_Window_Title, "Style/Background",
							End,

						End,
						Child, bt_grabstringcols = MUI_MakeObject( MUIO_Button, GS( PREFS_GRAB_GAD ) ),
					End,
					Child, VGroup,
						Child, VGroup, GroupFrameT( "URL underline" ),
							Child, ColGroup( 3 ),
								Child, HSpace( 0 ),
								Child, MUI_MakeObject( MUIO_Label, "Normal", MUIO_Label_Centered ),
								Child, MUI_MakeObject( MUIO_Label, "Selected", MUIO_Label_Centered ),
								Child, Label2( "Colour:" ),
								Child, data->pp_pens[ 10 ] = PoppenObject, MUIA_CycleChain, 1,
									MUIA_Pendisplay_Spec, "m5",
									MUIA_Window_Title, "URL/Normal",
								End,
								Child, data->pp_pens[ 11 ] = PoppenObject, MUIA_CycleChain, 1,
									MUIA_Pendisplay_Spec, "m6",
									MUIA_Window_Title, "URL/Selected",
								End,
							End,
/*
							Child, HGroup,
								Child, Label2( "Style:" ),
								Child, Cycle( underlineopts ),
							End,
*/
						End,
						Child, ColGroup( 2 ), GroupFrameT( "Misspelling underline" ),
							Child, Label2( "Colour:" ),
							Child, data->pp_pens[ 12 ] = PoppenObject, MUIA_CycleChain, 1,
								MUIA_Pendisplay_Spec, "m5",
								MUIA_Window_Title, "Misspelling",
							End,
/*
							Child, Label2( "Style:" ),
							Child, Cycle( underlineopts ),
*/
						End,

						Child, VGroup, GroupFrameT( "E-Mail Mode" ),
							Child, ColGroup( 3 ),
								Child, Label2( "Highlite quoted text?" ),
								Child, data->chk_mailquote = MUI_MakeObject( MUIO_Checkmark, "Highlite quoted text?" ),
								Child, HSpace( 0 ),
							End,
						End,
					End,
				End,
				Child, VGroup,

					Child, VSpace( 0 ),

					Child, ColGroup( 2 ), GroupFrameT( "Cursor" ),

						Child, Label1( "Style:" ),
						Child, data->cyc_cursorstyle = Cycle( cursoropts ),

						Child, Label1( "Pixels:" ),
						Child, data->nm_cursorsize = SliderObject,
							MUIA_Disabled, TRUE,
							MUIA_CycleChain, 1,
							MUIA_Numeric_Min, 1,
							MUIA_Numeric_Max, 10,
						End,

						Child, Label1( "Blink rate:" ),
						Child, data->nm_blinkrate = NewObject( numericclass->mcc_Class, NULL,
							MUIA_CycleChain, 1,
							MUIA_Numeric_Max, 60,
						End,
					End,

					Child, VSpace(0),

					Child, ColGroup(2), GroupFrameT( "Font" ),
						Child, Label1( "Normal:" ),
						Child, data->str_font = PopaslObject, MUIA_CycleChain, 1,
							MUIA_Popasl_Type, ASL_FontRequest,
							MUIA_Popstring_String, TextinputObject, StringFrame, End,
							MUIA_Popstring_Button, PopButton( MUII_PopFile ),
							ASLFO_TitleText, "Textinput normal font",
						End,

						Child, Label1( "Fixed:" ),
						Child, data->str_fixedfont = PopaslObject, MUIA_CycleChain, 1,
							MUIA_Popasl_Type, ASL_FontRequest,
							MUIA_Popstring_String, TextinputObject, StringFrame, End,
							MUIA_Popstring_Button, PopButton( MUII_PopFile ),
							ASLFO_TitleText, "Textinput fixed font",
						End,

					End,

					Child, VSpace( 0 ),

					Child, HGroup, GroupFrameT( "Button Images" ),
						Child, VGroup,
							Child, VSpace( 0 ),
							Child, Label1( "External Editor:" ),
							Child, VSpace( 0 ),
						End,
						Child, data->pi_editbut = MUI_NewObject( MUIC_Popimage, 
							MUIA_CycleChain, 1,
							MUIA_Imagedisplay_UseDefSize, TRUE,
							MUIA_Imagedisplay_Spec, "1:21",
							MUIA_Window_Title, "Image for 'External Editor' button",
						End,
					End,

					Child, VSpace( 0 ),

				End,


			End,

			Child, VGroup, GroupFrameT( "Key Control" ),

				Child, HGroup,
					Child, bt_defaultkeys = MUI_MakeObject( MUIO_Button, "Reset to default keys" ),
					Child, HSpace( 0 ),
/*
					Child, HGroup, GroupFrameT( "Mark qualifier(s)" ),
						Child, Label1( "Shift?" ),
						Child, MUI_MakeObject( MUIO_Checkmark, "Shift?" ),
						Child, Label1( "Control?" ),
						Child, MUI_MakeObject( MUIO_Checkmark, "Control?" ),
						Child, Label1( "Alt?" ),
						Child, MUI_MakeObject( MUIO_Checkmark, "Alt?" ),
					End,
*/
				End,

				Child, data->lv_keys = ListviewObject, MUIA_CycleChain, 1,
					MUIA_Listview_List, ListObject, InputListFrame,
						MUIA_List_DisplayHook, &disp_hook,
						MUIA_List_ConstructHook, &const_hook,
						MUIA_List_DestructHook, &dest_hook,
						MUIA_List_Title, TRUE,
						MUIA_List_Format, ",,",
					End,
				End,

				Child, HGroup,
					Child, HGroup,
						Child, data->str_hotkey = hotkeystring(),
						Child, BalanceObject,
						End,
						Child, Label1( "=" ),
						Child, data->po_action = PopobjectObject,
							MUIA_Popstring_String, data->txt_action = TextinputObject,
								TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_Textinput_NoInput, TRUE,
							End,
							MUIA_Popstring_Button, bt_action = PopButton( MUII_PopUp ),
							MUIA_Popobject_ObjStrHook, &objstr_hook,
							MUIA_Popobject_WindowHook, &window_hook,
							MUIA_Popobject_Object, data->lv_action = ListviewObject,
								MUIA_Listview_List, ListObject,
									InputListFrame,
									MUIA_List_SourceArray, actions,
								End,
							End,
						End,
					End,
					Child, data->bt_keyadd = MUI_MakeObject( MUIO_Button, GS( PREFS_ADD ) ),
					Child, data->bt_keydel = MUI_MakeObject( MUIO_Button, GS( PREFS_DEL ) ),
				End,

			End,

			Child, VGroup,
				Child, HGroup, GroupFrameT( "Text" ),
/*
					Child, Label1( "Tab spaces:" ),
					Child, MUI_MakeObject( MUIO_NumericButton, "C?", 1, 16, "%ld" ),
					Child, MUI_MakeObject( MUIO_VBar, 0 ),
*/
					Child, HSpace( 0 ), //TEMP!!!
					Child, Label1( "Wordwrap by default?" ),
					Child, data->chk_wordwrap = MUI_MakeObject( MUIO_Checkmark, "Wordwrap by default?" ),
					Child, Label1( "Column:" ),
					Child, data->nm_wordwrap = MUI_MakeObject( MUIO_NumericButton, "C?", 10, 120, "%ld" ),
					Child, HSpace( 0 ), //TEMP!!!
				End,

				Child, HGroup, GroupFrameT( "PopUp-Menus" ),
					Child, HSpace( 0 ),
					Child, Label1( "For single line?" ),
					Child, data->chk_popup_single = MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
					Child, Label1( "For multi line?" ),
					Child, data->chk_popup_multi = MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
				End,
/*
				Child, HGroup, GroupFrameT( "Show URLs" ),
					Child, HSpace( 0 ),
					Child, Label1( "For input gadgets?" ),
					Child, MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
					Child, Label1( "For no-input gadgets?" ),
					Child, MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
				End,
*/
				Child, ColGroup( 5 ), GroupFrameT( "Undo limits" ),
					Child, Label1( "For single line:" ),
#define numobj( min, max, def ) TextinputObject, StringFrame, MUIA_Textinput_IsNumeric, TRUE, MUIA_Textinput_Integer, def, MUIA_Textinput_MinVal, min, MUIA_Textinput_MaxVal, max, End
					Child, data->nm_undobytessingle = numobj( 0, 100000, 500 ),
					Child, Label1( "bytes or" ),
					Child, data->nm_undolevelssingle = numobj( 0, 10000, 50 ),
					Child, Label1( "levels" ),
					Child, Label1( "For multi line:" ),
					Child, data->nm_undobytesmulti = numobj( 0, 100000, 2000 ),
					Child, Label1( "bytes or" ),
					Child, data->nm_undolevelsmulti = numobj( 0, 10000, 200 ),
					Child, Label1( "levels" ),
				End,
			End,
			Child, VGroup,
				Child, VGroup, GroupFrameT( "External Editor" ),
					Child, HGroup,
						Child, Label2( GS( PREFS_EDITCALL ) ),
						Child, data->str_editor = PopaslObject, MUIA_CycleChain, 1,
							MUIA_Popasl_Type, ASL_FileRequest,
							MUIA_Popstring_String, TextinputObject, StringFrame, End,
							MUIA_Popstring_Button, PopButton( MUII_PopFile ),
							ASLFR_TitleText, "External editor",
							ASLFR_RejectIcons, TRUE,
						End,
					End,
					Child, HGroup,
						Child, HSpace( 0 ),
						Child, Label1( GS( PREFS_SYNC ) ),
						Child, data->chk_sync = MUI_MakeObject( MUIO_Checkmark, GS( PREFS_SYNC ) ),
						Child, HSpace( 0 ),
					End,
				End,
/*
				Child, HGroup, GroupFrameT( "Spell checking" ),
					Child, HSpace( 0 ),
					Child, Label1( "Spell checker" ),
					Child, Cycle( checkeropts ),
					Child, HSpace( 0 ),
					Child, Label1( "Auto-spell?" ),
					Child, MUI_MakeObject( MUIO_Checkmark, "Enable" ),
					Child, HSpace( 0 ),
				End,

				Child, HGroup, GroupFrameT( "Clickable URLs" ),
					Child, HSpace( 0 ),
					Child, Label1( "For input gadgets?" ),
					Child, MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
					Child, Label1( "For no-input gadgets?" ),
					Child, MUI_MakeObject( MUIO_Checkmark, "" ),
					Child, HSpace( 0 ),
				End,
*/
			End,

			Child, ColGroup( 2 ), GroupFrameT( "Example objects" ),

				Child, VCenter( Label2( "Single line:" ) ),
				Child, data->t1 = TextinputObject, StringFrame, MUIA_Textinput_Contents, "© 1997-2000 Oliver Wagner <owagner@vapor.com>", MUIA_CycleChain, 1, End,

				Child, VCenter( Label2( "NoInput:" ) ),
				Child, data->t3 = TextinputObject, TextFrame, MUIA_Background, MUII_TextBack, MUIA_Textinput_Contents, "© 1997-2000 Oliver Wagner <owagner@vapor.com>", MUIA_Textinput_NoInput, TRUE, End,

				Child, VCenter( Label2( "Multi line:" ) ),
				Child, data->t2 = TextinputscrollObject, StringFrame, MUIA_CycleChain, 1,
					MUIA_ShowMe, TRUE,
					MUIA_Textinput_Multiline, TRUE, 
					MUIA_Textinput_Contents, MLTEXT,
				End,
			End,

		End,
	End;

	if( !grp )
		return( 0 );

	DoMethod( obj, OM_ADDMEMBER, grp );

	SetAttrs( bt_grabstringcols, 
		MUIA_CycleChain, 1,
		MUIA_Font, MUIV_Font_Tiny,
		TAG_DONE
	);
	DoMethod( bt_grabstringcols, MUIM_Notify, MUIA_Pressed, FALSE,
		obj, 2, MUIM_Textinputmcp_GrabCols, FALSE
	);
	DoMethod( data->cyc_cursorstyle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, data->nm_cursorsize, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
	SetAttrs( bt_defaultkeys, MUIA_CycleChain, 1, TAG_DONE );
	DoMethod( bt_defaultkeys, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Textinputmcp_DefaultKeys );
	SetAttrs( data->cyc_cursorstyle, 
		MUIA_CycleChain, 1,
		TAG_DONE
	);

	// immediate changes
	DoMethod( data->nm_cursorsize, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		obj, 6, MUIM_MultiSet, MUIA_Textinput_CursorSize, MUIV_TriggerValue, data->t1, data->t2, NULL
	);

	// immediate changes
	DoMethod( data->nm_blinkrate, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
		obj, 6, MUIM_MultiSet, MUIA_Textinput_Blinkrate, MUIV_TriggerValue, data->t1, data->t2, NULL
	);

	// immediate changes
	DoMethod( data->cyc_cursorstyle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		obj, 6, MUIM_MultiSet, MUIA_Textinput_Cursorstyle, MUIV_TriggerValue, data->t1, data->t2, NULL
	);

	// Register gadgets
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->cyc_cursorstyle, MUICFG_Textinput_Cursorstyle, 1, "Cursorstyle" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_blinkrate, MUICFG_Textinput_Cursorstyle, 1, "Blinkrate" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->str_font, MUICFG_Textinput_Font, 1, "Font" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->str_fixedfont, MUICFG_Textinput_FixedFont, 1, "Fixed font" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->str_editor, MUICFG_Textinput_ExternalEditor, 1, "Editor call" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->pi_editbut, MUICFG_Textinput_ButtonImage, 1, "Image" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->chk_wordwrap, MUICFG_Textinput_WordWrapOn, 1, "Wordwrap" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_wordwrap, MUICFG_Textinput_WordWrapAt, 1, "Wordwrap" );
	for( c = 0; c < 13; c++ )
		DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->pp_pens[ c ], MUICFG_Textinput_Pens_Inactive_Foreground + c, 1, "Color" );

	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->chk_popup_single, MUICFG_Textinput_PopupSingle, 1, "Popup menus?" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->chk_popup_multi, MUICFG_Textinput_PopupMulti, 1, "Popup menus?" );

	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_undobytessingle, MUICFG_Textinput_UndoBytesSingle, 1, "Undo bytes" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_undobytesmulti, MUICFG_Textinput_UndoBytesMulti, 1, "Undo bytes" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_undolevelssingle, MUICFG_Textinput_UndoLevelsSingle, 1, "Undo levels" );
	DoMethod( obj, MUIM_Mccprefs_RegisterGadget, data->nm_undolevelsmulti, MUICFG_Textinput_UndoLevelsMulti, 1, "Undo levels" );

	DoMethod( obj, MUIM_Textinputmcp_LAct, MUIV_List_Active_Off );

	DoMethod( data->lv_keys, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
		obj, 2, MUIM_Textinputmcp_LAct, MUIV_TriggerValue
	);
	DoMethod( data->str_hotkey, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		obj, 1, MUIM_Textinputmcp_LCopy
	);
	DoMethod( data->lv_action, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
		data->po_action, 2, MUIM_Popstring_Close, TRUE
	);
	DoMethod( data->txt_action, MUIM_Notify, MUIA_Text_Contents, MUIV_EveryTime,
		obj, 1, MUIM_Textinputmcp_LCopy
	);
	DoMethod( data->bt_keyadd, MUIM_Notify, MUIA_Pressed, FALSE,
		obj, 1, MUIM_Textinputmcp_LAdd
	);
	DoMethod( data->bt_keydel, MUIM_Notify, MUIA_Pressed, FALSE,
		data->lv_keys, 2, MUIM_List_Remove, MUIV_List_Remove_Active
	);

	DoMethod( obj, MUIM_MultiSet, MUIA_HorizWeight, 0, data->bt_keyadd, data->bt_keydel, NULL );

	DoMethod( obj, MUIM_MultiSet, MUIA_CycleChain, 1,
		bt_action, data->bt_keyadd, data->bt_keydel, data->chk_sync,
		data->chk_wordwrap, data->nm_wordwrap,
		data->chk_popup_single, data->chk_popup_multi,
		data->nm_undobytessingle, data->nm_undobytesmulti,
		data->nm_undolevelssingle, data->nm_undolevelsmulti,
		data->chk_mailquote,
		NULL
	);

	return( (ULONG)obj );
}

static APTR getci( APTR obj, ULONG cid )
{
	APTR rv;

	rv = (APTR)DoMethod( obj, MUIM_Dataspace_Find, cid );
	return( rv );
}

static void setci( APTR obj, ULONG cid, APTR dat, ULONG size )
{
	DoMethod( obj, MUIM_Dataspace_Add, dat, size, cid );
}

DECMMETHOD( Settingsgroup_ConfigToGadgets )
{
	GETDATA;
	int c;
	ULONG *lv;
	STRPTR s;
	struct textinput_keydef *kd;

	// pens
	for( c = 0; c < 13; c++ )
	{
		struct MUI_PenSpec *ps;
		ps = getci( msg->configdata, MUICFG_Textinput_Pens_Inactive_Foreground + c );
		//Printf( "ps %ld %s %lx\n", c, ps, ps );
		if( ps )
		{
			set( data->pp_pens[ c ], MUIA_Pendisplay_Spec, ps );
			data->grabcol[ c ] = FALSE;
		}
		else
		{
			data->grabcols = TRUE;
			data->grabcol[ c ] = TRUE;
		}
	}

	lv = getci( msg->configdata, MUICFG_Textinput_Cursorstyle );
	if( lv )
		set( data->cyc_cursorstyle, MUIA_Cycle_Active, *lv );

	lv = getci( msg->configdata, MUICFG_Textinput_CursorSize );
	if( lv )
		set( data->nm_cursorsize, MUIA_Numeric_Value, *lv );

	lv = getci( msg->configdata, MUICFG_Textinput_Blinkrate );
	if( lv )
		set( data->nm_blinkrate, MUIA_Numeric_Value, *lv );

	lv = getci( msg->configdata, MUICFG_Textinput_HiliteQuotes );
	if( lv )
		set( data->chk_mailquote, MUIA_Selected, *lv );
	else
		set( data->chk_mailquote, MUIA_Selected, TRUE );

	s = getci( msg->configdata, MUICFG_Textinput_Font );
	if( s )
		set( data->str_font, MUIA_String_Contents, s );

	s = getci( msg->configdata, MUICFG_Textinput_FixedFont );
	if( s )
		set( data->str_fixedfont, MUIA_String_Contents, s );

	s = getci( msg->configdata, MUICFG_Textinput_ExternalEditor );
	if( s )
		set( data->str_editor, MUIA_String_Contents, s );

	s = getci( msg->configdata, MUICFG_Textinput_ButtonImage );
	if( s )
		set( data->pi_editbut, MUIA_Imagedisplay_Spec, s );

	if( data->grabcols )
	{
		if( muiGlobalInfo( obj ) )
			DoMethod( obj, MUIM_Textinputmcp_GrabCols, TRUE );
	}

	lv = getci( msg->configdata, MUICFG_Textinput_KeyCount );
	if( lv )
	{
		int c;
		set( data->lv_keys, MUIA_List_Quiet, TRUE );
		DoMethod( data->lv_keys, MUIM_List_Clear );
		for( c = 0; c < *lv; c++ )
		{
			kd = getci( msg->configdata, MUICFG_Textinput_KeyBase + c );
			if( kd )
				DoMethod( data->lv_keys, MUIM_List_InsertSingle, kd, MUIV_List_Insert_Bottom );
		}
		set( data->lv_keys, MUIA_List_Quiet, FALSE );
	}
	else
	{
		DoMethod( obj, MUIM_Textinputmcp_DefaultKeys );
	}

	lv = getci( msg->configdata, MUICFG_Textinput_EditSync );
	if( lv )
		set( data->chk_sync, MUIA_Selected, *lv );

	lv = getci( msg->configdata, MUICFG_Textinput_WordWrapOn );
	set( data->chk_wordwrap, MUIA_Selected, lv ? *lv : TRUE );
	lv = getci( msg->configdata, MUICFG_Textinput_WordWrapAt );
	set( data->nm_wordwrap, MUIA_Numeric_Value, lv ? *lv : 70 );

	lv = getci( msg->configdata, MUICFG_Textinput_PopupSingle );
	set( data->chk_popup_single, MUIA_Selected, lv ? *lv : FALSE );
	lv = getci( msg->configdata, MUICFG_Textinput_PopupMulti );
	set( data->chk_popup_multi, MUIA_Selected, lv ? *lv : TRUE );

	lv = getci( msg->configdata, MUICFG_Textinput_UndoBytesSingle );
	set( data->nm_undobytessingle, MUIA_Textinput_Integer, lv ? *lv : 500 );
	lv = getci( msg->configdata, MUICFG_Textinput_UndoBytesMulti );
	set( data->nm_undobytesmulti, MUIA_Textinput_Integer, lv ? *lv : 2000 );
	lv = getci( msg->configdata, MUICFG_Textinput_UndoLevelsSingle );
	set( data->nm_undolevelssingle, MUIA_Textinput_Integer, lv ? *lv : 50 );
	lv = getci( msg->configdata, MUICFG_Textinput_UndoLevelsMulti );
	set( data->nm_undolevelsmulti, MUIA_Textinput_Integer, lv ? *lv : 200 );

	return( 0 );
}

DECMMETHOD( Settingsgroup_GadgetsToConfig )
{
	GETDATA;
	STRPTR s;
	ULONG v;
	int c;

	// pens
	for( c = 0; c < 13; c++ )
	{
		struct MUI_PenSpec *ps;
		get( data->pp_pens[ c ], MUIA_Pendisplay_Spec, &ps );
		setci( msg->configdata, MUICFG_Textinput_Pens_Inactive_Foreground + c, ps, sizeof( *ps ) );
	}

	get( data->cyc_cursorstyle, MUIA_Cycle_Active, &v );
	setci( msg->configdata, MUICFG_Textinput_Cursorstyle, &v, sizeof( v ) );

	get( data->nm_cursorsize, MUIA_Numeric_Value, &v );
	setci( msg->configdata, MUICFG_Textinput_CursorSize, &v, sizeof( v ) );

	get( data->nm_blinkrate, MUIA_Numeric_Value, &v );
	setci( msg->configdata, MUICFG_Textinput_Blinkrate, &v, sizeof( v ) );

	get( data->chk_sync, MUIA_Selected, &v );
	setci( msg->configdata, MUICFG_Textinput_EditSync, &v, sizeof( v ) );

	get( data->str_font, MUIA_String_Contents, &s );
	setci( msg->configdata, MUICFG_Textinput_Font, s, strlen( s ) + 1 );

	get( data->str_fixedfont, MUIA_String_Contents, &s );
	setci( msg->configdata, MUICFG_Textinput_FixedFont, s, strlen( s ) + 1 );

	get( data->str_editor, MUIA_String_Contents, &s );
	setci( msg->configdata, MUICFG_Textinput_ExternalEditor, s, strlen( s ) + 1 );

	get( data->pi_editbut, MUIA_Imagedisplay_Spec, &s );
	setci( msg->configdata, MUICFG_Textinput_ButtonImage, s, strlen( s ) + 1 );

	get( data->chk_wordwrap, MUIA_Selected, &v );
	setci( msg->configdata, MUICFG_Textinput_WordWrapOn, &v, sizeof( v ) );

	get( data->chk_mailquote, MUIA_Selected, &v );
	setci( msg->configdata, MUICFG_Textinput_HiliteQuotes, &v, sizeof( v ) );

	get( data->nm_wordwrap, MUIA_Numeric_Value, &v );
	setci( msg->configdata, MUICFG_Textinput_WordWrapAt, &v, sizeof( v ) );

	// keys
	get( data->lv_keys, MUIA_List_Entries, &v );
	setci( msg->configdata, MUICFG_Textinput_KeyCount, &v, sizeof( v ) );
	for( c = 0; c < v; c++ )
	{
		struct textinput_keydef *kd;
		DoMethod( data->lv_keys, MUIM_List_GetEntry, c, &kd );
		if( kd )
			setci( msg->configdata, MUICFG_Textinput_KeyBase + c, kd, sizeof( *kd ) );
	}

	{
		static UWORD verrev[] = { VERSION, REVISION };
		setci( msg->configdata, MUICFG_Textinput_SavedVerRev, verrev, 4 );
	}

	get( data->chk_popup_single, MUIA_Selected, &v );
	setci( msg->configdata, MUICFG_Textinput_PopupSingle, &v, sizeof( v ) );

	get( data->chk_popup_multi, MUIA_Selected, &v );
	setci( msg->configdata, MUICFG_Textinput_PopupMulti, &v, sizeof( v ) );

	return( 0 );
}

DECMMETHOD( Textinputmcp_GrabCols )
{
	GETDATA;
	static ULONG defid[ 13 ] = { MUICFG_StringColor, MUICFG_StringBack, MUICFG_StringActiveColor, MUICFG_StringActiveBack, MUICFG_StringActiveBack, MUICFG_StringActiveColor, MUICFG_ActiveObjectPen, MUICFG_StringColor, MUICFG_StringColor, MUICFG_ActiveObjectPen, MUICFG_StringActiveColor, MUICFG_StringColor, MUICFG_StringActiveColor };
	int c;

	for( c = 0; c < 13; c++ )
	{
		if( !msg->notall || data->grabcol[ c ] )
		{
			struct MUI_PenSpec *ps;
			char pensp[ 32 ];
			int pen;

			if( c == 6 )
			{
				DoMethod( obj, MUIM_GetConfigItem, MUICFG_StringActiveColor, &ps );
				pen = MUI_ObtainPen( muiRenderInfo( obj ), ps, 0 );
				sprintf( pensp, "p%ld", ~pen );
				MUI_ReleasePen( muiRenderInfo( obj ), pen );
				ps = (struct MUI_PenSpec*)pensp;
			}
			else if( c == 7 )
			{
				DoMethod( obj, MUIM_GetConfigItem, MUICFG_StringActiveBack, &ps );
				pen = MUI_ObtainPen( muiRenderInfo( obj ), ps, 0 );
				sprintf( pensp, "p%ld", ~pen );
				MUI_ReleasePen( muiRenderInfo( obj ), pen );
				ps = (struct MUI_PenSpec*)pensp;
			}
			else if( c == 8 )
			{
				get( data->pp_pens[ 7 ], MUIA_Pendisplay_Spec, &ps );
			}
			else if( c == 9 )
			{
				get( data->pp_pens[ 6 ], MUIA_Pendisplay_Spec, &ps );
			}
			else if( c == 10 || c == 12 )
			{
				get( data->pp_pens[ 2 ], MUIA_Pendisplay_Spec, &ps );
			}
			else if( c == 11 )
			{
				get( data->pp_pens[ 7 ], MUIA_Pendisplay_Spec, &ps );
			}
			else
				DoMethod( obj, MUIM_GetConfigItem, defid[ c ], &ps );

			set( data->pp_pens[ c ], MUIA_Pendisplay_Spec, ps );
		}
	}

	return( 0 );
}

DECMMETHOD( Textinputmcp_DefaultKeys )
{
	GETDATA;
	struct textinput_keydef *kd;

	set( data->lv_keys, MUIA_List_Quiet, TRUE );
	DoMethod( data->lv_keys, MUIM_List_Clear );

	get( data->t1, MUIA_Textinput_DefKeys, &kd );
	while( kd->action != -1 )
	{
		DoMethod( data->lv_keys, MUIM_List_InsertSingle, kd, MUIV_List_Insert_Bottom );
		kd++;
	}
	set( data->lv_keys, MUIA_List_Quiet, FALSE );

	return( 0 );
}

DECMMETHOD( Setup )
{
	GETDATA;

	if( data->grabcols )
	{
		DoMethod( obj, MUIM_Textinputmcp_GrabCols, TRUE );
		data->grabcols = FALSE;
	}

	return( DOSUPER );
}

DECMMETHOD( Textinputmcp_LAct )
{
	GETDATA;

	if( msg->which == MUIV_List_Active_Off )
	{
		DoMethod( obj, MUIM_MultiSet, MUIA_Disabled, TRUE,
			data->str_hotkey, data->po_action, data->bt_keydel,
			NULL
		);
	}
	else
	{
		struct textinput_keydef *kd;

		DoMethod( data->lv_keys, MUIM_List_GetEntry, msg->which, &kd );

		SetAttrs( data->txt_action,
			MUIA_NoNotify, TRUE,
			MUIA_Text_Contents, actions[ kd->action ],
			TAG_DONE
		);
		set( data->lv_action, MUIA_List_Active, kd->action );
		set( data->po_action, MUIA_Disabled, FALSE );
		SetAttrs( data->str_hotkey,
			MUIA_NoNotify, TRUE,
			MUIA_String_Contents, kd->key,
			MUIA_Disabled, FALSE,
			TAG_DONE
		);
		set( data->bt_keydel, MUIA_Disabled, FALSE );
	}

	return( 0 );
}

DECMMETHOD( Textinputmcp_LCopy )
{
	GETDATA;
	struct textinput_keydef *kd;
	char *p;
	ULONG v;

	DoMethod( data->lv_keys, MUIM_List_GetEntry,  MUIV_List_GetEntry_Active, &kd );
	if( !kd )
		return( 0 );

	get( data->str_hotkey, MUIA_String_Contents, &p );
	get( data->lv_action, MUIA_List_Active, &v );

	strcpy( kd->key, p );
	kd->action = v;

	DoMethod( data->lv_keys, MUIM_List_Redraw, MUIV_List_Redraw_Active );

	return( 0 );
}

DECMMETHOD( Textinputmcp_LAdd )
{
	GETDATA;
	struct textinput_keydef kd;

	memset( &kd, 0, sizeof( kd ) );
	DoMethod( data->lv_keys, MUIM_List_InsertSingle, &kd, MUIV_List_Insert_Bottom );
	set( data->lv_keys, MUIA_List_Active, MUIV_List_Active_Bottom );
	return( 0 );
}

__asm __saveds ULONG dispfunc(
	register __a0 struct IClass *cl,
	register __a2 Object *obj,
	register __a1 Msg msg
)
{
	switch( msg->MethodID )
	{
		DEFMMETHOD( Settingsgroup_ConfigToGadgets )
		DEFMMETHOD( Settingsgroup_GadgetsToConfig )
		DEFMMETHOD( Textinputmcp_GrabCols )
		DEFMMETHOD( Textinputmcp_DefaultKeys )
		DEFMMETHOD( Textinputmcp_LAct )
		DEFMMETHOD( Textinputmcp_LCopy )
		DEFMMETHOD( Textinputmcp_LAdd )
		DEFMMETHOD( Setup )
		DEFMETHOD( OM_NEW )
	}
	return( DOSUPER );
}

#include "prefsicon.c"

static APTR prefsiconobj( void )
{
	return BitmapObject,
		MUIA_Bitmap_Bitmap, &prefsiconBitMap,
		MUIA_Bitmap_Width, 25,
		MUIA_Bitmap_Height, 10,
		MUIA_Bitmap_SourceColors, prefsiconCMap32,
		MUIA_Bitmap_Transparent, 0,
		MUIA_FixWidth, 25,
		MUIA_FixHeight, 10,
		End;
}

//
// blink rate gadget
//

DECMMETHOD2( Numeric_Stringify )
{
	static char res[ 12 ];

	if( !msg->value )
		return( (ULONG)"\033c-Off-" );

	sprintf( res, "\033c%ldms", msg->value * 50 );

	return( (ULONG)res );
}

static __asm __saveds ULONG dispfunc_num(
	register __a0 struct IClass *cl,
	register __a2 Object *obj,
	register __a1 Msg msg
)
{
	switch( msg->MethodID )
	{
		DEFMMETHOD2( Numeric_Stringify )
	}
	return( DOSUPER );
}

BOOL ClassInitFunc( struct Library *base )
{
	#if INCLUDE_VERSION < 44
	LocaleBase = OpenLibrary( "locale.library", 38 );
	#else
	LocaleBase = (struct LocaleBase *)OpenLibrary( "locale.library", 38 );
	#endif
	if( LocaleBase )
	{
		catalog = OpenCatalog( NULL, "Textinput_mcc.catalog", 
			OC_BuiltInLanguage, "english",
			TAG_DONE
		);
	}

	if( MUIMasterBase->lib_Version >= 20 )
		ThisClassP->mcc_Class->cl_ID = "Textinput.mcp";

	numericclass = MUI_CreateCustomClass( NULL, MUIC_Slider, NULL, 4, dispfunc_num );
	return( (BOOL)(numericclass ? TRUE : FALSE ) );
}

VOID ClassExitFunc( struct Library *base )
{
	if( LocaleBase )
	{
		CloseCatalog( catalog );
		catalog = NULL;
		CloseLibrary( LocaleBase );
		LocaleBase = NULL;
	}
	if( numericclass )
	{
		MUI_DeleteCustomClass( numericclass );
		numericclass = NULL;
	}
}
