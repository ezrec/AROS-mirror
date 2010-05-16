/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_version.c
 */

extern CONST_STRPTR vers;
extern CONST_STRPTR vstring;
extern CONST_STRPTR verstag;
extern CONST_STRPTR version_date;
extern CONST_STRPTR release;
extern const ULONG version_ulong;
extern const ULONG revision_ulong;

// transforms a define into a string
#define STR(x)  STR2(x)
#define STR2(x) #x

#if defined(__AROS__)
    #if defined __i386__
        #define CPU         "[AROS/i386]"
    #elif defined __x86_64__
        #define CPU         "[AROS/x86_64]"
    #elif defined __mc68000__
        #define CPU         "[AROS/68K]"
    #elif defined __powerpc__
        #define CPU         "[AROS/PPC]"
    #elif defined __arm__
        #define CPU         "[AROS/arm]"
    #else
        #define CPU         "[AROS]"
    #endif
#elif defined(__PPC__)
    #if defined(__amigaos4__)
        #define CPU         "[OS4/PPC]"
    #elif defined(__MORPHOS__)
        #define CPU         "[MOS/PPC]"
    #else
        #define CPU         "[PPC]"
    #endif
#else
    #define CPU             "[OS3/68k]"
#endif

#if defined(__GNUC__)
  #if defined(__GNUC_PATCHLEVEL__)
    #define COMPILER "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
  #else
    #define COMPILER "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) ".x"
  #endif
#elif defined(__SASC)
  #define COMPILER "SAS/C " STR(__VERSION__) "." STR(__REVISION__)
#else
  #define COMPILER "unknown"
  #warning "unknown compiler specification"
#endif

#define PROGNAME            "Scout"
#define RELEASEVERSION      "3"
#define RELEASEREVISION     "7"
#define COPYRIGHT           "  ©2004-2010 Scout Open Source Team  \n  ©2001-2003 Thore Böckelmann  \n  ©1994-1997 Andreas Gelhausen, Richard Körber  "
#define AUTHOR              "Thore Böckelmann"
#define DESCRIPTION         "System monitor"
#define HOMEPAGE            "http://sourceforge.net/projects/scoutos/"

