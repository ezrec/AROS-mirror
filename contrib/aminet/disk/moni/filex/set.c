#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/reqtools.h>

#include <proto/exec.h>

#include <string.h>
#include <stdio.h>
#endif

#include "filexstructs.h"
#include "filexstrings.h"
										/* prototypes */
#include "allprotos.h"

void SetDisplayTyp( short new, struct DisplayData *DD )
{
	if(( new != DD->DisplayTyp ) || ( DD->DisplayTyp == 3 ))
	{
		DD->DisplayTyp = new;

		RedrawDisplay( DD );
	}

	SetDisplayCheckMarks();
}

void SetRequester(BOOL new)
{
	if( new != UseAsl)
	{
			/* Falls reqtools.library gebraucht werden soll, diese ggf. öffnen */

		if( !new)
			if( !ReqToolsBase )
			{
				char errortext[80];

				if( !( ReqToolsBase = ( struct ReqToolsBase * )OpenLibrary( REQTOOLSNAME, 38 )))
				{
						/* reqtools.library konnte nicht geöffnen werden */
						/* Fehler melden und weiterhin asl gebrauchen */

					sprintf(errortext, GetStr( MSG_INFO_GLOBAL_CANTOPENLIBRARY ), REQTOOLSNAME );

					MyRequestNoLocale( errortext, 0 );
					return;
				}
			}

		FreeRequester();

		UseAsl = new;

		AllocRequester();
	}
}

void SetARexxCommand(ULONG Number, UBYTE *Name)
{
	UBYTE fullname[256]="Rexx:";

	if( Number == -1 )
	{
		Number = 0;

		if( !GetWert( &Number, MSG_WINDOWTITLE_ENTERCOMMANDNUMBER, 0, 9 ))
			return;
	}

	if(!Name)
	{
		Name = fullname;
		if(!MyRequestFile( Name, GetStr( MSG_WINDOWTITLE_ENTERCOMMANDNUMBER ), 0, FALSE ))
			return;
	}

	SetARexxMenu( Number, Name );
	if( Scr ) SetNewMenus();
}

void SetCommandShellWindow(char *string)
{
	UBYTE *s;

	if((s = AllocVec(strlen(string)+1,MEMF_ANY)))
	{
		FreeVec(arexxcommandshellwindow);

		strcpy(arexxcommandshellwindow = s,string);
	}
}

void SetCommandWindow(char *string)
{
	UBYTE *s;

	if((s = AllocVec(strlen(string)+1,MEMF_ANY)))
	{
		FreeVec(arexxcommandwindow);

		strcpy(arexxcommandwindow = s,string);
	}
}
