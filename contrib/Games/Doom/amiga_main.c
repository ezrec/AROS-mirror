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
// Revision 1.1  2000/02/29 18:21:03  stegerg
// Doom port based on ADoomPPC. Read README.AROS!
//
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

const char amigaversion[] = "$VER: ADoomPPC 1.3 " __AMIGADATE__ ;

long __oslibversion = 38;	/* we require at least OS3.0 for LoadRGB32() */
char __stdiowin[] = "CON:0/20/640/140/ADoomPPC";
char __stdiov37[] = "/AUTO/CLOSE/WAIT";

#include <stdio.h>
#include <stdlib.h>

#include <exec/exec.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <PowerUP/ppclib/ppc.h>

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
    "-avg"
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

  printf ("%s\n", &amigaversion[6]);

  if (argc == 0)
  {
  /* run from WB, _WBArgv is name and TOOLTYPEs */
    argc = _WBArgc;
    argv = _WBArgv;
  }
  myargc = argc;
  if ((myargv = malloc(sizeof(char *)*MAXARGVS)) == NULL)
    I_Error ("malloc(%d) failed", sizeof(char *)*MAXARGVS);
  memset (myargv, 0, sizeof(char *)*MAXARGVS);
  memcpy (myargv, argv, sizeof(char *)*myargc);

  printf ("\nADoomPPC parameters are:\n\n    ");
  for (i = 1; i < myargc; i++)
    printf (" %s", myargv[i]);
  printf ("\n\n");

  cpu_type = PPCGetAttr(PPCINFOTAG_CPU);
  p = M_CheckParm ("-cpu");
  if (p && p < myargc - 1) {
    cpu_type = atoi (myargv[p+1]);
  }
  switch (cpu_type) {
    case 3:
      printf ("\nCPU is PPC603 ");
      break;
    case 4:
      printf ("\nCPU is PPC604 ");
      break;
    case 5:
      printf ("\nCPU is PPC602 ");
      break;
    case 6:
      printf ("\nCPU is PPC603e ");
      break;
    case 7:
      printf ("\nCPU is PPC603e+ ");
      break;
    case 8:
      printf ("\nCPU is PPC604e ");
      break;
    default:
      printf ("\nCPU is PPC ");
      break;
  }

  bus_clock = PPCGetAttr(PPCINFOTAG_CPUCLOCK);
  printf ("running at %d MHz ", bus_clock);
  if (!bus_clock)
    bus_clock = 50000000;
  else
    bus_clock = bus_clock * 1000000;
  i = PPCGetAttr(PPCINFOTAG_CPUPLL);
  switch (i) {
    case 0:
    case 1:
    case 2:
    case 3:
      pll = 1.0;			// PLL is 1:1 (or bypassed)
      break;
    case 4:
    case 5:
      pll = 2.0;			// PLL is 2:1
      break;
    case 8:
    case 9:
      pll = 3.0;			// PLL is 3:1
      break;
    case 12:
      if ((cpu_type == 4) || (cpu_type == 8))
        pll = 1.5;			// PLL is 1.5:1
      else
        pll = 4.0;			// PLL is 4:1
      break;
    case 13:
      pll = 4.0;			// PLL is 4:1
      break;
    default:
      pll = 3.0;
      break;
  }
  printf ("using a PLL divisor of %3.1f.\n", pll);

  bus_clock = (int)((double)bus_clock / pll);
  p = M_CheckParm ("-bus");
  if (p && p < myargc - 1) {
    bus_clock = atoi (myargv[p+1]);
  }
  bus_MHz = bus_clock / 1000000;
  printf("Bus clock is %d MHz.\n\n", bus_MHz);

  tb_scale_lo = ((double)(bus_clock >> 2)) / 35.0;
  tb_scale_hi = (4.294967296E9 / (double)(bus_clock >> 2)) * 35.0;

  SetFPMode ();  /* set FPU rounding mode to "trunc towards -infinity" */

  p = M_CheckParm ("-forceversion");
  if (p && p < myargc - 1)
    VERSION = atoi (myargv[p+1]);

  D_DoomMain ();

  return 0;
}
