/*  title_clock_prefs.c
 *
 * $Date$
 * $Revision$
 *
 *  40.3
 *  20010804    DM  + Fixed a couple of enforcer hits on startup in interface.c/ShowExample()
 *                  when trying to use the default locale settings before the GUI was opened.
 *  40.2
 *  20010804    DM  + Added main loop processing
 *
 *  40.1
 *  20010720    DM  + Bumped version to be in line with the new plugin API (tested and works :).
 *                  Increased required OS version to 3.0 (since that's waht Sclaos needs)
 *
 *  39.1
 *  20010716    DM  + Created, only has skeleton code for reading prefs file, printing
 *                  contents and opening/closing libraries.
 *
 *
 *  To do:
 *      Get it compiling properly with SAS/C
 *      Parse any arguments passed as either WBArgs or CLI
 *      Save myself as the default tool for the project icons
 *      MUI GUI?
 *      Bugfixes :)
 *
 */

#include <exec/types.h>


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/icon.h>

#include <stdio.h>
#include <string.h>

#include <defs.h>
#include "title_clock_prefs.h"
#include "prefs_file.h"
#include "interface.h"
#include "tcp_locale.h"


/* Version string */
#define VERSION_STR     "40"
#define REVISION_STR    "3"
#define DATE_STR        " (04/08/01)"
char version[] = "$VER: title_clock.prefs " VERSION_STR "." REVISION_STR DATE_STR " ©2001 The Scalos Team";

char    datestr[FORMAT_SIZE] = {0};     /* String for entering date format string - can be small since it should just be a few %<x>'s */
char    timestr[FORMAT_SIZE] = {0};     /* as above, for time */

char    date_ex[EXAMPLE_SIZE] = {0};    /* Some space to store the example date */
char    time_ex[EXAMPLE_SIZE] = {0};    /* as above, for time */

char    date_re[FORMAT_SIZE] = {0};     /* Copy of the date loaded at the start for restore */
char    time_re[FORMAT_SIZE] = {0};     /* Copy of time " */

/* Global variables for library bases */
struct IntuitionBase    *IntuitionBase;
struct GfxBase          *GfxBase;
struct Library          *GadToolsBase;
T_LOCALEBASE       	LocaleBase;
T_UTILITYBASE	      	UtilityBase;
struct Library          *DiskfontBase;
struct Library          *AslBase;       /* Open/Save As requesters */
struct Library          *IconBase;      /* For saving icons */

#ifdef __amigaos4__
struct IntuitionIFace	*IIntuition;
struct GraphicsIFace	*IGraphics;
struct GadToolsIFace	*IGadTools;
struct LocaleIFace	*ILocale;
struct UtilityIFace	*IUtility;
struct DiskfontIFace	*IDiskfont;
struct AslIFace		*IAsl;
struct IconIFace	*IIcon;
#endif

/* Opens all required libraries. If one of the essential libraries
 * cannot be opened, it will return 0, else non-zero. Non-essential
 * libraries for this program are locale and utility.
 */
int OpenLibs(void)
{
	/* Open essential libraries */
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
	if(!IntuitionBase) return(0);
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if(!IIntuition) return(0);
#endif

	GadToolsBase = OpenLibrary("gadtools.library", 39);
	if(!GadToolsBase) return(0);
#ifdef __amigaos4__
	IGadTools = (struct GadToolsIFace *)GetInterface(GadToolsBase, "main", 1, NULL);
	if(!IGadTools) return(0);
#endif

	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 36);
	if(!GfxBase) return(0);
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	if(!IGraphics) return(0);
#endif

	/* Open non-essential libraries (do not return failure for any of these) */
	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 0);
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 0);
	DiskfontBase =OpenLibrary("diskfont.library", 0);
	AslBase = OpenLibrary("asl.library", 36);

#ifdef __amigaos4__
	if (LocaleBase) ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
	if (UtilityBase) IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (DiskfontBase) IDiskfont = (struct DiskfontIFace *)GetInterface(DiskfontBase, "main", 1, NULL);
	if (AslBase) IAsl = (struct AslIFace *)GetInterface(AslBase, "main", 1, NULL);
#endif

	if(LocaleBase) tcp_Catalog = OpenCatalog(NULL, (STRPTR)"title_clock_prefs.catalog", OC_BuiltInLanguage, "english", TAG_DONE);

	return(1);
}


/* Closes all opened libraries and sets base pointers to NULL */
void CloseLibs(void)
{
#ifdef __amigaos4__
	if(IAsl) DropInterface((struct Interface *)IAsl);
	IAsl = NULL;
#endif
	if(AslBase) CloseLibrary(AslBase);
	AslBase = NULL;

#ifdef __amigaos4__
	if(IGraphics) DropInterface((struct Interface *)IGraphics);
	IGraphics = NULL;
#endif
	if(GfxBase) CloseLibrary((struct Library *)GfxBase);
	GfxBase = NULL;

#ifdef __amigaos4__
	if(IGadTools) DropInterface((struct Interface *)IGadTools);
	IGadTools = NULL;
#endif
	if(GadToolsBase) CloseLibrary(GadToolsBase);
	GadToolsBase = NULL;

#ifdef __amigaos4__
	if(IIntuition) DropInterface((struct Interface *)IIntuition);
	IIntuition = NULL;
#endif
	if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	IntuitionBase = NULL;

#ifdef __amigaos4__
	if(IUtility) DropInterface((struct Interface *)IUtility);
	IUtility = NULL;
#endif
	if(UtilityBase) CloseLibrary((struct Library *)UtilityBase);
	UtilityBase = NULL;

	if(LocaleBase)
	{
		if(tcp_Catalog) CloseCatalog(tcp_Catalog);
		tcp_Catalog = NULL;

#ifdef __amigaos4__
		if(ILocale) DropInterface((struct Interface *)IAsl);
		ILocale = NULL;
#endif
		CloseLibrary((struct Library *)LocaleBase);
	}
	LocaleBase = NULL;
}


/* Tries to force plugin to re-read prefs. Will work if the plugin has been
 * OpenLibrary'd already (since it will be in the exec lists as title_clock.plugin
 * i.e. without the path, so we can open it easily.
 */
void    ForcePluginRead(void)
{
	struct Library  *PluginBase;

	if(NULL != (PluginBase = OpenLibrary("title_clock.plugin", 40)) )
	{
		//printf("plugin poened\n");
//		tcpl_ReReadPrefs();
		CloseLibrary(PluginBase);
	}
}


int main(int argc, char **argv)
{
	ULONG   winmask, mask, sigs;
	WORD    quit;

	//printf("argc=%d\n", argc);

	if(!OpenLibs()) return(20);

	/* Try to read prefs file and if that doesn't happen, set it to the default
	 * (blank strings will be interpreted as use the standard locale strings)
	 */
	if(!LoadPrefs(datestr, FORMAT_SIZE, timestr, FORMAT_SIZE))
	{
		datestr[0] = '\0'; timestr[0] = '\0';
	}

	/* Take copies for the restore function */
	strcpy(date_re, datestr);
	strcpy(time_re, timestr);

	//printf("Blah %s\n", datestr);
	//printf("Blah %s\n", timestr);


	/*
	strcpy(datestr, "");
	strcpy(timestr, "");
	SavePrefs(datestr, timestr);
	ForcePluginRead();
	*/

	quit = 0;
	if(OpenMainWindow())
	{
		while(!quit)
		{
			if(tcp_Window) winmask = (1L << tcp_Window->UserPort->mp_SigBit); else winmask=0L;
			mask =  winmask | SIGBREAKF_CTRL_C;
			sigs = Wait(mask);

			if(sigs & winmask) quit = HandleMainMessages();
			if(sigs & SIGBREAKF_CTRL_C) quit = 1;
		}
	}
	CloseMainWindow();  // yes, it should be here, because it needs to close any partial opens too

	CloseLibs();

	return 0;
}


/* Not needed for vbcc */
#if 0
/* Main function run when program is run from WB */
int wbmain(struct WBStartup *wbs)
{
	return(main(0, (char **)wbs));
}
#endif

