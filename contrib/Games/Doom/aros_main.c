// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log$
// Revision 1.2  2004/09/25 16:54:23  stegerg
// More fixes/enhancements by Joseph Fenton.
//
// Revision 1.1  2000/02/29 18:21:03  stegerg
// Doom port based on ADoomPPC. Read README.AROS!
//
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

const char arosversion[] = "$VER: AROS Doom 1.3 (24.02.2000)";

long __oslibversion = 38;	/* we require at least OS3.0 for LoadRGB32() */
char __stdiowin[] = "CON:0/20/640/140/AROS Doom";
char __stdiov37[] = "/AUTO/CLOSE/WAIT";

#include <stdio.h>
#include <stdlib.h>

#include <exec/exec.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>

#include <proto/exec.h>
#include <proto/icon.h>

#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"
#include "i_system.h"
#include "m_fixed.h"

/**********************************************************************/

int VERSION = 110;

int cpu_type;
int bus_clock;
int bus_MHz;
double tb_scale_lo;
double tb_scale_hi;

/**********************************************************************/
extern int _WBArgc;
extern char **_WBArgv;

int main (int argc, char* argv[])
{ 
  int i, p;
  double pll;

  /* these command line arguments are flags */
  static char *flags[] = {
    "-forcedemo",
    "-changepitch",
    "-mouse",
    "-joypad",
    "-music",
    "-70Hz",
    "-140Hz",
    "-nosfx",
    "-mmu",
    "-fps",
    "-rotatemap",
    "-maponhu",
//    "-rtg",
//    "-native",
//    "-ehb",
    "-mousepointer",
    "-sega3",
    "-sega6",
    "-pcchecksum",
    "-revrseipx",
    "-directcgx",
//    "-graffitilo",
//    "-graffitihi",
    "-rawkey",
    "-maxdemo",
    "-nodraw",
    "-noblit",
    "-debugfile",
    "-shdev",
    "-regdev",
    "-comdev",
    "-nomonsters",
    "-respawn",
    "-fast",
    "-devparm",
    "-altdeath",
    "-deathmatch",
    "-cdrom",
    "-playdemo",
    "-avg",
    "-scr"
  };
  /* these command line arguments each take a value */
  static char *settings[] = {
    "-screenmode",
//    "-taskpriority",
    "-heapsize",
    "-cpu",
    "-bus",
    "-forceversion",
    "-width",
    "-height", 
    "-waddir",
    "-file",
    "-deh",
    "-timedemo",
    "-skill",
    "-episode",
    "-timer",
    "-statcopy",
    "-record",
    "-playdemo",
    "-timedemo",
    "-loadgame",
    "-config",
    "-turbo"
  };

  printf ("%s\n", &arosversion[6]);

  if (argc == 0)
  {
#ifndef AROS
  /* run from WB, _WBArgv is name and TOOLTYPEs */
    argc = _WBArgc;
    argv = _WBArgv;
#endif
  }
  myargc = argc;
  if ((myargv = malloc(sizeof(char *)*MAXARGVS)) == NULL)
    I_Error ("malloc(%d) failed", sizeof(char *)*MAXARGVS);
  memset (myargv, 0, sizeof(char *)*MAXARGVS);
  memcpy (myargv, argv, sizeof(char *)*myargc);

  printf ("\nAROS Doom parameters are:\n\n    ");
  for (i = 1; i < myargc; i++)
    printf (" %s", myargv[i]);
  printf ("\n\n");

  p = M_CheckParm ("-forceversion");
  if (p && p < myargc - 1)
    VERSION = atoi (myargv[p+1]);

  D_DoomMain ();

  return 0;
}
