/*
 *		TESTCALLS.C												vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		This file simply performs a variety of calls to every function
 *		which can be patched by SnoopDos. It can be asked to do specific
 *		calls, or it can march through the whole lot.
 *
 *		It's used to ensure that our patch code executes correctly.
 *
 *		Usage: TestCalls [opts] [funcs to test]
 */

#include "system.h"

char Version[] = "$VER: TestCalls 1.1 (19.3.94)";

char HelpMsg[] = 
"TestCalls 1.0 by Eddy Carroll. Performs function calls to test SnoopDos.\n"
"\n"
"Usage: TestCalls [-fail | -succeed] [#] [Dos | System | All | <names> ]\n"
"\n"
"-fail    means only call functions with parameters known to return failure.\n"
"-succeed means only call functions with parameters known to return success.\n"
"\n"
"If a number (#) is specified, the tests are repeated that number of times.\n"
"Dos, System and All select the function groups to be tested. Instead of\n"
"selecting one of these groups, you can also list one or more function\n"
"names from the following list:\n"
"\n"
"System functions:\n"
"\n"
"  FindPort      FindSemapore  LockScreen    OpenFont       OpenResource\n"
"  FindResident  FindTask      OpenDevice    OpenLibrary    ReadToolTypes\n"
"\n"
"DOS functions:\n"
"\n"
"  CurrentDir    GetVar        MakeDir       Rename         System\n"
"  DeleteFile    LoadSeg       MakeLink      RunCommand\n"
"  Execute       Lock          Open          SetVar\n"
"\n";

#define END_FUNC	0
#define SYS_FUNC	(1 << 0)
#define DOS_FUNC	(2 << 0)
#define ALL_FUNC	(SYS_FUNC | DOS_FUNC)

typedef void (*TestFunc)(int, int);

/*
 *		Now the test functions
 */
#define TF(name)	void Test_##name(int succ, int fail) {}

TF(CurrentDir)
TF(DeleteFile)
TF(Execute)
TF(GetVar)
TF(LoadSeg)
TF(Lock)
TF(MakeDir)
TF(MakeLink)
TF(Rename)
TF(RunCommand)
TF(SetVar)
TF(System)

/*
 *		Open
 */
void Test_Open(int trysucc, int tryfail)
{
	BPTR file;
	char uniquename[100];

	sprintf(uniquename, "Ram:Testfile-%08x", FindTask(0));

	if (trysucc) {
		file = Open(uniquename, MODE_NEWFILE);
		if (file)
			Close(file);
		else
			printf("Warning: Open(\"%s\", write) failed.\n", uniquename);
		file = Open(uniquename, MODE_OLDFILE);
		if (file)
			Close(file);
		else
			printf("Warning: Open(\"%s\", read) failed.\n", uniquename);
		file = Open(uniquename, MODE_READWRITE);
		if (file)
			Close(file);
		else
			printf("Warning: Open(\"%s\", modify) failed.\n", uniquename);
	}
	if (tryfail) {
		file = Open("ram:X/Y/Z/TestCalls-test", MODE_NEWFILE);
		if (file) {
			printf("Warning: Open(\"Ram:X/Y/Z/TestCalls-test\", write) "
				   "succeeded.\n");
			Close(file);
		}
		file = Open("ram:X/Y/Z/TestCalls-test", MODE_OLDFILE);
		if (file) {
			printf("Warning: Open(\"Ram:X/Y/Z/TestCalls-test\", read) "
				   "succeeded.\n");
			Close(file);
		}
		file = Open("ram:X/Y/Z/TestCalls-test", MODE_READWRITE);
		if (file) {
			printf("Warning: Open(\"Ram:X/Y/Z/TestCalls-test\", modify) "
				   "succeeded.\n");
			Close(file);
		}
	}
}

/*
 *		FindPort
 */
void Test_FindPort(int trysucc, int tryfail)
{
	if (trysucc) {
		if (!FindPort("AREXX"))
			printf("Warning: FindPort(\"AREXX\") failed.\n");
	}
	if (tryfail) {
		if (FindPort("Illegal Port Name"))
			printf("Warning: FindPort(\"Illegal Port Name\") succeeded.\n");
	}
}

/*
 *		FindResident
 */
void Test_FindResident(int trysucc, int tryfail)
{
	if (trysucc) {
		if (!FindResident("con-handler"))
			printf("Warning: FindResident(\"con-handler\") failed.\n");
	}
	if (tryfail) {
		if (FindResident("Illegal Module"))
			printf("Warning: FindResident(\"Illegal Module\") succeeded.\n");
	}
}

/*
 *		FindSemaphore
 */
void Test_FindSemaphore(int trysucc, int tryfail)
{
	if (trysucc) {
		if (!FindSemaphore("« IPrefs »"))
			printf("Warning: FindSemaphore(\"« IPrefs »\") failed.\n");
	}
	if (tryfail) {
		if (FindSemaphore("Illegal Semaphore"))
			printf("Warning: "
				   "FindSemaphore(\"Illegal Semaphore\") succeeded.\n");
	}
}

/*
 *		FindTask. We check to make sure FindTask(NULL) is ignored also.
 */
void Test_FindTask(int trysucc, int tryfail)
{
	if (trysucc) {
		FindTask(NULL);
		if (!FindTask("DF0"))
			printf("Warning: FindPort(\"DF0\") failed.\n");
	}
	if (tryfail) {
		if (!trysucc)
			FindTask(NULL);
		if (FindTask("Illegal task"))
			printf("Warning: FindTask(\"Illegal task\") succeeded.\n");
	}
}

/*
 *		LockPubScreen. Also check to make sure LockPubScreen(NULL) is ignored.
 */
void Test_LockScreen(int trysucc, int tryfail)
{
	void *scr;

	if (trysucc) {
		scr = LockPubScreen(NULL);
		if (scr)
			UnlockPubScreen(NULL, scr);
		scr = LockPubScreen("Workbench");
		if (scr)
			UnlockPubScreen(NULL, scr);
		else
			printf("Warning: LockPubScreen(\"Workbench\") failed.\n");
	}
	if (tryfail) {
		if (!trysucc) {
			scr = LockPubScreen(NULL);
			if (scr)
				UnlockPubScreen(NULL, scr);
		}
		scr = LockPubScreen("Illegal Screen");
		if (scr) {
			printf("Warning: LockPubScreen(\"Illegal Screen\") succeeded.\n");
			UnlockPubScreen(NULL, scr);
		}
	}
}

/*
 *		OpenDevice.
 */
void Test_OpenDevice(int trysucc, int tryfail)
{
	struct MsgPort *port;
	struct IORequest *ioreq;

	port = CreateMsgPort();
	if (!port) {
		printf("Couldn't create message port for OpenDevice()\n");
		return;
	}
	ioreq = CreateIORequest(port, sizeof(*ioreq));
	if (!ioreq) {
		printf("Couldn't create IO request for OpenDevice()\n");
		DeleteMsgPort(port);
		return;
	}
	if (trysucc) {
		if (OpenDevice("timer.device", UNIT_VBLANK, ioreq, 0) == 0)
			CloseDevice(ioreq);
		else
			printf("Warning: OpenDevice(\"timer.device\") failed.\n");
	}
	if (tryfail) {
		if (OpenDevice("Illegal device", 0, ioreq, 0) == 0)
			printf("Warning: OpenDevice(\"Illegal device\") succeeded.\n");
	}
	DeleteIORequest(ioreq);
	DeleteMsgPort(port);
}

/*
 *		OpenFont.
 */
void Test_OpenFont(int trysucc, int tryfail)
{
	struct TextFont *font;
	struct TextAttr textattr;

	textattr.ta_Name  = "topaz.font";
	textattr.ta_YSize = 8;
	textattr.ta_Style = FS_NORMAL;
	textattr.ta_Flags = 0;

	if (trysucc) {
		font = OpenFont(&textattr);
		if (font)
			CloseFont(font);
		else
			printf("Warning: OpenFont(\"topaz 8\") failed.\n");
	}
	if (tryfail) {
		textattr.ta_Name = "Illegal font";
		font = OpenFont(&textattr);
		if (font) {
			printf("Warning: OpenFont(\"Illegal 8\") succeeded.\n");
			CloseFont(font);
		}
	}
}

/*
 *		OpenLibrary.
 */
void Test_OpenLibrary(int trysucc, int tryfail)
{
	void *libbase;

	if (trysucc) {
		libbase = OpenLibrary("dos.library", 33);
		if (libbase)
			CloseLibrary(libbase);
		else
			printf("Warning: OpenLibrary(\"dos.library\", 33) failed.\n");
	}
	if (tryfail) {
		libbase = OpenLibrary("Illegal library", 0);
		if (libbase) {
			printf("Warning: OpenLibrary(\"Illegal library\", 0) succeeded.\n");
			CloseLibrary(libbase);
		}
	}
}

/*
 *		OpenResource.
 */
void Test_OpenResource(int trysucc, int tryfail)
{
	if (trysucc) {
		if (!OpenResource("ciaa.resource"))
			printf("Warning: OpenResource(\"ciaa.resource\") failed.\n");
	}
	if (tryfail) {
		if (OpenResource("Illegal resource"))
			printf("Warning: OpenResource(\"Illegal resource\") succeeded.\n");
	}
}

/*
 *		FindToolTypes
 */
void Test_ReadToolTypes(int trysucc, int tryfail)
{
	static char *tooltypes[] = {
		"ToolType 1",
		"ToolType 2",
		"ToolType 3",
		NULL
	};
	char *tooltype = "VALUE1|VALUE2|VALUE3";

	/*
	 *		First try FindToolType()
	 */
	if (trysucc) {
		if (!FindToolType(tooltypes, "ToolType 1"))
			printf("Warning: FindToolType(\"ToolType 1\") failed.\n");
	}
	if (tryfail) {
		if (FindToolType(tooltypes, "Illegal ToolType"))
			printf("Warning: FindToolType(\"Illegal ToolType\") succeeded.\n");
	}
	/*
	 *		Now check MatchToolValue()
	 */
	if (trysucc) {
		if (!MatchToolValue(tooltype, "VALUE1"))
			printf("Warning: MatchToolValue(\"VALUE1\") failed.\n");
	}
	if (tryfail) {
		if (MatchToolValue(tooltype, "Illegal Value"))
			printf("Warning: MatchToolValue(\"Illegal Value\") succeeded.\n");
	}
}

/*****************************************************************************
 *
 *		Now the table used to call the test functions
 * 
 *****************************************************************************/

struct {
	int		 type;
	char	 *name;
	TestFunc func;
} FuncTable[] = {

#define FT(type,name)	type, #name, Test_##name

	FT(SYS_FUNC, FindPort),
	FT(SYS_FUNC, FindResident),
	FT(SYS_FUNC, FindSemaphore),
	FT(SYS_FUNC, FindTask),
	FT(SYS_FUNC, LockScreen),
	FT(SYS_FUNC, OpenDevice),
	FT(SYS_FUNC, OpenFont),
	FT(SYS_FUNC, OpenLibrary),
	FT(SYS_FUNC, OpenResource),
	FT(SYS_FUNC, ReadToolTypes),

	FT(DOS_FUNC, CurrentDir),
	FT(DOS_FUNC, DeleteFile),
	FT(DOS_FUNC, Execute),
	FT(DOS_FUNC, GetVar),
	FT(DOS_FUNC, LoadSeg),
	FT(DOS_FUNC, Lock),
	FT(DOS_FUNC, MakeDir),
	FT(DOS_FUNC, MakeLink),
	FT(DOS_FUNC, Open),
	FT(DOS_FUNC, Rename),
	FT(DOS_FUNC, RunCommand),
	FT(DOS_FUNC, SetVar),
	FT(DOS_FUNC, System),
	END_FUNC, NULL, NULL
};

/*
 *		Mainline
 */
int main(int argc, char **argv)
{
	int trysucceed = 1;
	int tryfail    = 1;
	int numrepeats = 1;

	if (CheckSignal(SIGBREAKF_CTRL_C)) {
		printf("^C\n");
		exit(20);
	}
	while (argc > 1 && argv[1][0] == '-') {
		switch (tolower(argv[1][1])) {
			case 'h':	printf(HelpMsg); exit(0);
			case 'f':	trysucceed = 0;	 tryfail	= 1;	break;
			case 's':	tryfail    = 0;	 trysucceed	= 1;	break;
			default:
				printf("Unknown option %s -- type TestCalls ? for help\n",
						argv[1]);
				break;
		}
		argv++;
		argc--;
	}
	if (argc > 1 && atoi(argv[1]) > 0) {
		numrepeats = atoi(argv[1]);
		argv++;
		argc--;
	}
	if (argc == 1) {
		printf("No functions specified. Type TestCalls ? for help.\n");
		exit(10);
	}
	if (argv[1][0] == '?') {
		printf(HelpMsg);
		exit(0);
	}

	/*
	 *		Now parse command line options to decide what functions to test
	 */
	while (numrepeats-- > 0) {
		int argi;

		for (argi = 1; argi < argc; argi++) {
			char *opt = argv[argi];
			int mask = 0;

			if (stricmp(opt, "system") == 0)
				mask = SYS_FUNC;
			else if (stricmp(opt, "dos") == 0)
				mask = DOS_FUNC;
			else if (stricmp(opt, "all") == 0)
				mask = ALL_FUNC;

			if (mask) {
				int i;

				for (i = 0; FuncTable[i].type != END_FUNC; i++) {
					if (FuncTable[i].type & mask)
						FuncTable[i].func(trysucceed, tryfail);
					if (SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
						printf("^C\n");
						exit(20);
					}
				}
			} else {
				int i;

				for (i = 0; FuncTable[i].type != END_FUNC; i++) {
					if (stricmp(opt, FuncTable[i].name) == 0) {
						FuncTable[i].func(trysucceed, tryfail);
						break;
					}
				}
				if (FuncTable[i].type == END_FUNC) {
					printf("Warning: Unrecognised function name '%s' "
						   "ignored.\n", opt);
				}
			}
		}
	}
	return (0);
}
