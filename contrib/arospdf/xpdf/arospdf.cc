//========================================================================
//
// AROSpdf.cc
//
// Copyright 2003 Glyph & Cog, LLC
// Copyright 2005 Vijay Kumar B. <vijaykumar@bravegnu.org>
//
//========================================================================

#include <aconf.h>
#include <stdio.h>

#include "parseargs.h"
#include "gmem.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Splash.h"
#include "AROSPDFApp.h"
#include "config.h"
#define ARG_TEMPLATE "FILE"


static char enableT1libStr[16] = "";
static char enableFreeTypeStr[16] = "";
static char antialiasStr[16] = "";
static char ownerPassword[33] = "";
static char userPassword[33] = "";
static GBool quiet = gFalse;
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
#if HAVE_T1LIB_H
  {"-t1lib",      argString,      enableT1libStr, sizeof(enableT1libStr),
   "enable t1lib font rasterizer: yes, no"},
#endif
#if HAVE_FREETYPE_FREETYPE_H | HAVE_FREETYPE_H
  {"-freetype",   argString,      enableFreeTypeStr, sizeof(enableFreeTypeStr),
   "enable FreeType font rasterizer: yes, no"},
#endif
  {"-aa",         argString,      antialiasStr,   sizeof(antialiasStr),
   "enable font anti-aliasing: yes, no"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-q",      argFlag,     &quiet,         0,
   "don't print any messages or errors"},
  {"-cfg",        argString,      cfgFileName,    sizeof(cfgFileName),
   "configuration file to use in place of .xpdfrc"},
  {"-v",      argFlag,     &printVersion,  0,
   "print copyright and version info"},
  {"-h",      argFlag,     &printHelp,     0,
   "print usage information"},
  {"-help",   argFlag,     &printHelp,     0,
   "print usage information"},
  {"--help",  argFlag,     &printHelp,     0,
   "print usage information"},
  {"-?",      argFlag,     &printHelp,     0,
   "print usage information"},
  {NULL}
};

int main(int argc, char *argv[]) {
  AROSPDFApp *app;
  GString *fileName;
  GString *ownerPW, *userPW;
  GBool ok;
  BPTR cd=NULL;
  int exitCode;

  exitCode = 99;
	fileName=NULL;
	if (argc>1) {
		// parse args
		ok = parseArgs(argDesc, &argc, argv);
		if (!ok || printVersion || printHelp) {
			fprintf(stderr, "AROSpdf version %s\n", xpdfVersion);
			fprintf(stderr, "%s\n", xpdfCopyright);
			if (!printVersion) {
				printUsage("AROSpdf", "<PDF-file>", argDesc);
			}
			goto err0;
		}
		if (argc==2) 
		fileName = new GString(argv[1]);
		else   fileName=NULL;
	}
	
	if (argc==0) {
		fileName=NULL;
		struct WBStartup *startup = NULL;
		startup = (struct WBStartup *) argv;
		if (startup != NULL) {
			if (startup->sm_NumArgs >1)
			{
				/* FIXME: all arguments but the first are ignored */
				cd = CurrentDir(startup->sm_ArgList[1].wa_Lock);
				fileName = new GString(startup->sm_ArgList[1].wa_Name);
			}
		}
	}


  // read config file
  globalParams = new GlobalParams(cfgFileName);
  globalParams->setupBaseFonts(NULL);
  if (enableT1libStr[0]) {
    if (!globalParams->setEnableT1lib(enableT1libStr)) {
      fprintf(stderr, "Bad '-t1lib' value on command line\n");
    }
  }
  if (enableFreeTypeStr[0]) {
    if (!globalParams->setEnableFreeType(enableFreeTypeStr)) {
      fprintf(stderr, "Bad '-freetype' value on command line\n");
    }
  }
  if (antialiasStr[0]) {
    if (!globalParams->setAntialias(antialiasStr)) {
      fprintf(stderr, "Bad '-aa' value on command line\n");
    }
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // open PDF file
  if (ownerPassword[0]) {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0]) {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  
  if (fileName != NULL)
  app = new AROSPDFApp(fileName, ownerPW, userPW);
	else
	    app = new AROSPDFApp();
  if (!app->isOk()) {
    bug("Not ok?\n");
    exitCode = 1;
    goto err1;
  }
  exitCode = app->run();
  app->quit();
 err1:
  delete app;
  delete globalParams;
 err0:
  // check for memory leaks
  xObject::memCheck(stderr);
  gMemReport(stderr);
  if (cd != NULL)  CurrentDir(cd);
  return exitCode;
}
