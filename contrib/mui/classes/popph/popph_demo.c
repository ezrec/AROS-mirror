
/*
** $Id$
**
** Popplaceholder.mcc - demonstration program
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
**                        http://amiga.com.pl/mcc/
*/

/// Includes

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <exec/types.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>

#include <mui/Popplaceholder_mcc.h>

LONG __stack = 18192;

//#define SAVEDS __saveds
//#define ASM __asm
//#define REG(x) register __ ## x

//|

char VER[] = "$VER: PopplaceholderDemo 15.0 (23.11.99)";

/// CleanUp
void CleanUp(void)
{
   if(MUIMasterBase)   {CloseLibrary(MUIMasterBase); MUIMasterBase = NULL; }
}
//|
/// xget
ULONG xget( Object *obj, int attr)
{
ULONG val;

		get( obj, attr, &val);
		return( val );
}
//|

/// Main

static const char *PP_Table[] =
{
	"%s|normal",
	"%g|highlight",
	"%x|3-dimensional",
	"%w|4-dimensional",
	"Tag|Description",
	"WWW|World Wide Web",
	NULL
};

static const char *PP_Table_Single[] =
{
		"something 1",
		"something 2",
		"something 3",
		NULL
};

struct  Library *MUIMasterBase  = NULL;

#define TextObj(a) TextObject, MUIA_Text_Contents, (a), MUIA_Font, MUIV_Font_Tiny, End


int main(void)
{

APTR app, window, pph;


   if( !(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, 19)) )
	   {
	   printf("Can't open %s v%ld\n", MUIMASTER_NAME, (long)19);
	   CleanUp();
	   return(EXIT_FAILURE);
	   }

   app = ApplicationObject,
			MUIA_Application_Title      , "Popplaceholder Demo",
//            MUIA_Application_Version    , VERSTAG,
			MUIA_Application_Copyright  , "©1999 Marcin Orlowski <carlos@amiga.com.pl>",
			MUIA_Application_Author     , "Marcin Orlowski",
			MUIA_Application_Base       , "POPPLACEHOLDERDEMO",
			MUIA_Application_Window     ,
			   window = WindowObject,
						MUIA_Window_ID, 0,
						MUIA_Window_Title, "Popplaceholder demo",
						WindowContents,
							  VGroup,
							  Child, TextObj("\033cOrdinary object"),
							  Child, pph = PopphObject,
									 MUIA_Popph_Array      , PP_Table,
									 MUIA_Popph_Contents   , "Something",
//                                     MUIA_Popph_ReplaceMode, TRUE,
									 End,


							  Child, TextObj("\033cAnd PopAsl alike"),
							  Child, PopphObject,
									 MUIA_Popph_Contents   , "300 chars limit",
									 MUIA_Popph_Array      , PP_Table,
									 MUIA_Popph_PopAsl     , TRUE,
									 MUIA_Popph_Contents   , "RAM:",
									 ASLFR_DoSaveMode      , TRUE,
									 End,


							  Child, TextObj("\033cAnother PopAsl (drawers only)"),
							  Child, PopphObject,
									 MUIA_Popph_Array      , PP_Table,
									 MUIA_Popph_PopAsl     , TRUE,
									 MUIA_Popph_Contents   , "SYS:",
									 ASLFR_DrawersOnly     , TRUE,
									 MUIA_Listview_ScrollerPos, MUIV_Listview_ScrollerPos_None,
									 End,


							  Child, TextObj("\033cAlways 'classic' string (we Avoid others)"),
							  Child, PopphObject,
									 MUIA_Popph_Array      , PP_Table,
									 MUIA_Popph_Avoid      , MUIV_Popph_Avoid_Textinput | MUIV_Popph_Avoid_Betterstring,
									 End,

/*
							  // not implemented yet!
							  Child, TextObject, MUIA_Text_Contents, "\033cLet's override the string object", End,
							  Child, PopphObject,
									 MUIA_Popph_Array       , PP_Table,
									 MUIA_Popph_StringObject, StringObject, End,
									 End,
*/

							  Child, TextObj("\033cList with column title"),
							  Child, PopphObject,
									 MUIA_Popph_Array      , PP_Table,
									 MUIA_Popph_Title      , "\033bCol1|\033u\033iCol2",
									 MUIA_Popph_Contents   , "Nothing ;-)",
									 End,

							  Child, TextObj("\033cSingle column..."),
							  Child, PopphObject,
									 MUIA_Popph_Array       , PP_Table_Single,
															MUIA_Popph_SingleColumn, TRUE,
									 MUIA_Popph_Title       , "\033bTitle",
									 MUIA_Popph_Contents    , "... with replace mode on",
									 MUIA_Popph_ReplaceMode , TRUE,
									 End,



							  End,
						End,

				 End;



	  if(app)
		{
		DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);



		set(window,MUIA_Window_Open,TRUE);

		if( xget(window, MUIA_Window_Open) )
		   {
		   ULONG sigs = 0;

		   while(DoMethod(app,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit)
			 {
			 if(sigs)
			   {
			   sigs = Wait(sigs | SIGBREAKF_CTRL_C);
			   if(sigs & SIGBREAKF_CTRL_C) break;
			   }
			 }
		   }
		else
		   {
		   printf("Can't open window!\n");
		   }

//              printf( "Result string: '%s'\n", xget(pph, MUIA_Popph_Contents) );

		MUI_DisposeObject(app);
		}
	  else
		{
		printf("Can't create application object\n");
		DisplayBeep(NULL);
		}

   CleanUp();
   return(EXIT_SUCCESS);
}
//|
