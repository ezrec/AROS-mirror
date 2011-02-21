/* BGUI test program */
/* Sorry, I am not able to compile and test this. I have VBCC on my hard
   drive, but it's too much hassle to set it up with the BGUI header files.
   I would like to know about the bugs in this code. */
/* TAB=2 */

#include <stdlib.h>

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <proto/bgui.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <stdio.h>

#define ID_QUIT 1

#define ARG_TITLE 0
#define ARG_DD 1
#define ARG_MULTI 2
#define ARG_COUNT 3

IPTR args[ARG_COUNT];
void *rda=NULL;
struct Library *BGUIBase=NULL;
Object *window=NULL;
struct Window *wnd;
struct IntuitionBase * IntuitionBase;

void cleanexit(ULONG rc,STRPTR text) {
	if (window) DisposeObject(window);
	if (BGUIBase) CloseLibrary(BGUIBase);
	if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if (rda) FreeArgs(rda);
	if (text) FPrintf(Output(),"Error: could not %s.\n",text);
	exit((int) rc);
}

void read_args(void) {
	rda=ReadArgs("TITLE=TITLES/S,D&D=DRAG&DROP/S,MULTI=MULTISELECT/S",args,NULL);
	if (!rda) {
		PrintFault(IoErr(),"BGUI_Test");
		cleanexit(20,NULL);
	}
}

BOOL open_all(void) {
	STRPTR stupid_c_temp_mxlabels[]={"MX object created with the PrefMx macro.",
		"Main label through the object!","Force LAB_NoPlaceIn, please.",NULL};
	STRPTR stupid_c_temp_entries[]={"1\tStart me with \"title\"",
		"2\tto see titles and bug #1.",
		"3\t\x1B""b\x1BiEnforcer hit:",
		"4\tbyte r/w $00000008",
		"5\tOmit \"title\" to see that",
		"6\tmulticolumn listviews",
		"7\teven with drag&drop",
		"8\tare handled without hits.",
		"9\tBug #2:",
		"10\tHave D&D off and drag",
		"11\tthe mouse up. It moves",
		"12\tup and down. (try",
		"13\tmultiselect too!)",
		"14\t\x1BuPossible cause:",
		"15\tnegative number (above)",
		"16\tand 0xFFFF (convert",
		"17\tLONG->UWORD->LONG)",
		"18\tis below the list.",
		NULL};
	LONG stupid_c_temp_cw[]={100,1000};

	if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
	{
	  printf("Could not open Intuition.library!\n");
	  return FALSE;
	}

	BGUIBase=OpenLibrary("bgui.library",41);
	if (!BGUIBase) cleanexit(20,"open BGUI V41+");
	window=WindowObject,
		WINDOW_Title,"BGUI test program",
		WINDOW_AutoAspect,TRUE,
		WINDOW_Position,POS_CENTERMOUSE,
		WINDOW_ScaleWidth,30,
		WINDOW_ScaleHeight,0,
		WINDOW_MasterGroup,
			VGroupObject,NormalSpacing,HOffset(4),VOffset(4),
			GROUP_BackFill,SHINE_RASTER,
				StartMember,PrefInfo("Prefs",
"The filename for preferences is not\n"
"determined correctly when starting from\n"
"CLI. BGUI always uses ln_Name. For CLI\n"
"processes, use FilePart() of\n"
"GetProgramName(buffer,length). (This\n"
"returns FALSE if not a CLI process.)",NULL,6),EndMember,
				StartMember,PrefMx("MX",stupid_c_temp_mxlabels,0,0),EndMember,
				StartMember,ListviewObject,
					LISTV_EntryArray,stupid_c_temp_entries, /* With E I can place
										   that list directly
										   here. */
					LISTV_Columns,2,
					BT_DragObject,args[ARG_DD],
					BT_DropObject,args[ARG_DD],
					args[ARG_TITLE] ? LISTV_Title : TAG_IGNORE,"N\xBA\tInfo",
					LISTV_DragColumns,TRUE,
					LISTV_ShowDropSpot,args[ARG_DD],
					LISTV_ColumnWeights,stupid_c_temp_cw,
					LISTV_MultiSelect,args[ARG_MULTI],
					LAB_Place,PLACE_ABOVE,
				EndObject,EndMember,
				StartMember,HGroupObject,Spacing(4),
					VarSpace(DEFAULT_WEIGHT),
					StartMember,PrefButton("_Quit",ID_QUIT),EndMember,
					VarSpace(DEFAULT_WEIGHT),
				EndObject,FixMinHeight,EndMember,
		EndObject,
	EndObject;
	if (!window) cleanexit(20,"create GUI");
	if (!WindowOpen(window)) cleanexit(20,"open window");

	return TRUE;
}

void event_loop(void) {
	ULONG code;
	IPTR sig;
	UBYTE going=TRUE;
	GetAttr(WINDOW_SigMask,window,&sig);
	while (going) {
		while ((code=HandleEvent(window))!=WMHI_NOMORE) {
			switch (code) {
				case ID_QUIT:
				case WMHI_CLOSEWINDOW:
				going=FALSE;
			}
		}
		if (going) Wait(sig);
	}
}

int main(int argc, char **argv) {
	read_args();
	open_all();
	event_loop();
	cleanexit(0,NULL);
        return 0;
}
