#define _USE_SYSBASE
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#include "textinput_mcc.h"

struct Library *MUIMasterBase;

APTR app, win, ti1, ti2, timl1, timl2, bt_get;

#define C1 "The quick brown fox jumps over the lazy dog"
#define C2 "Registered Software License Agreement\n"\
"-------------------------------------\n\n"\
"This agreement governs the license for the software packages\n"\
"AmIRC, AmigaNCP, AmFTP, AmTALK, AmTerm, AmTelnet, Voyager and\n"\
"MicroDot, thereafter refered to as \"the product\".\n\n"\
"AmIRC, AmigaNCP, AmTALK, Voyager and MicroDot are\n"\
"Copyright © 1996-97 by Oliver Wagner, All Rights Reserved.\n\n"\
"AmFTP, AmTerm and AmTelnet are\n"\
"Copyright © 1996-97 by Mathias Mischler, All Rights Reserved.\n\n"\
"AmIRC documentation and AmFTP documentation\n"\
"are Copyright © 1996-97 by Laurence Walton, All Rights Reserved.\n\n"\
"The producer of this software will grant the Licensee a limited,\n"\
"non-exclusive right to use the product and it's associated files on\n"\
"a single machine. The producer will hand out a special identification\n"\
"file called the \"key file\" to the Licensee. This key file will\n"\
"enable the Licensee to fully use the product package according to\n"\
"the terms of this license.\n\n"\
"The key file is strictly for personal use by the Licensee only.\n"\
"Any disclosure of the key file will invalidate the license and\n"\
"the key file itself. The Licensee will be held liable of any\n"\
"damage arising out of the disclosure of the key file.\n\n"\
"The product is provided \"as is\" without warranty of any kind,\n"\
"either express or implied, statutory or otherwise, including\n"\
"without limitation any implied warranties of non-infringement,\n"\
"merchantability and fitness for a particular purpose.\n"\
"The entire risk as to use, results and performance of the product\n"\
"is assumed by the Licensee and should the product prove to be\n"\
"defective, the Licensee assume the entire cost of all necessary\n"\
"servicing, repair or other remediation.\n\n"\
"In no event shall the producer of this product or it's resellers\n"\
"be liable for any property damage, personal injury, loss of use\n"\
"or other indirect, incidental or consequential damages, including\n"\
"without limitation, any damages for lost profits, business\n"\
"interruption  or data which may be lost or rendered inaccurate,\n"\
"even if the producer has been advised of the possibility of such\n"\
"damages.\n\n"\
"This agreement shall exclusively be governed by the laws of the\n"\
"Federal Republic of Germany."


void main( void )
{
	int Done = FALSE;
	ULONG sig;
	static STRPTR styleoptions[] = { "IRC", "Email", "HTML", NULL };
	if( !( MUIMasterBase = OpenLibrary( MUIMASTER_NAME, 19 ) ) )
		return;

	if( app = ApplicationObject,
		MUIA_Application_Title, "TextInputDemo",
		MUIA_Application_Version, "$VER: TextInput Demo 2.0",
		MUIA_Application_Copyright, "© 1997-99 by Oliver Wagner, All Rights Reserved",
		MUIA_Application_Author, "Oliver Wagner",
		MUIA_Application_Description, "Demo for Textinput.mcc",
		MUIA_Application_Base, "TEXTINPUT-DEMO",
		SubWindow, win = WindowObject,
			MUIA_Window_ID, 42,
			MUIA_Window_Title, "TextInput Demo",
			WindowContents, VGroup,
				Child, HGroup,
					Child, Label1( "Style:" ),
					Child, Cycle( styleoptions ),
				End,
				Child, HGroup, GroupFrameT( "Single line" ),
					Child, ColGroup( 2 ),
						Child, Label2( "Input:" ),
						Child, ti1 = TextinputObject, StringFrame, MUIA_CycleChain, 1,
							MUIA_ControlChar, 'a',
							MUIA_Textinput_Multiline, FALSE, 
							MUIA_Textinput_Contents, C1,
						End,
						Child, Label2( "NoInput:" ),
						Child, ti2 = TextinputObject, TextFrame, 
							MUIA_Background, MUII_TextBack,
							MUIA_Textinput_NoInput, TRUE,
							MUIA_Textinput_Contents, C1,
						End,
					End,
				End,
				Child, VGroup, GroupFrameT( "Multi line with scroll" ),
					Child, timl1 = TextinputscrollObject, StringFrame, MUIA_CycleChain, 1,
						MUIA_ControlChar, 'b',
						MUIA_Textinput_Multiline, TRUE, 
						MUIA_Textinput_Contents, C2,
					End,
					Child, bt_get = SimpleButton( "Get contents (to shell window)" ),
				End,
				Child, HGroup, GroupFrameT( "NoInput multi line with Scroll" ),
					Child, timl2 = TextinputscrollObject, TextFrame, MUIA_Background,MUII_TextBack,MUIA_CycleChain, 1,
						MUIA_ControlChar, 'c',
						MUIA_Textinput_NoInput, TRUE,
						MUIA_Textinput_Multiline, TRUE, 
						MUIA_Textinput_AutoExpand, TRUE,
						MUIA_Textinput_Contents, C2,
						MUIA_Textinput_MinVersion, 12,
						MUIA_Textinput_WordWrap, 60,
					End,
				End,
				Child, HGroup, GroupFrameT( "Old stringgadget" ),
					Child, String( C1, 256 ),
				End,
			End,
		End,
	End )
	{
		DoMethod( win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
		);
		DoMethod( bt_get, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, 2
		);
		DoMethod( ti1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
			ti2, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue
		);
		DoMethod( timl1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
			timl2, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue
		);
		DoMethod( ti1, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			ti1, 3, MUIM_Set, MUIA_Textinput_Contents, ""
		);
		set( win, MUIA_Window_Open, TRUE );
		while( !Done )
		{
			LONG id;
			id = DoMethod( app, MUIM_Application_Input, &sig );
			switch( id )
			{
				case MUIV_Application_ReturnID_Quit:
					Done = TRUE;
				case 2:
					{
						char *p;
						get( timl1, MUIA_Textinput_Contents, &p );
						PutStr( ">>>>>\n" );
						PutStr( p );
						PutStr( "<<<<<\n" );
					}
					break;
			}
			if( sig )
				if( Wait( sig | SIGBREAKF_CTRL_C ) & SIGBREAKF_CTRL_C )
					Done = TRUE;
		}
		MUI_DisposeObject( app );
	}
	CloseLibrary( MUIMasterBase );

}
