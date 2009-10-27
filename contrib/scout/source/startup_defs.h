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
 * You must not use this source code to gain profit of any kind!
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#ifndef STARTUP_DEFS_H
#define STARTUP_DEFS_H

#define MYLIBVERSION        37

typedef STRPTR  KEY;
typedef LONG *  NUMBER;
typedef LONG    SWITCH;

struct Args {
   NUMBER   ToolPri;
   KEY      Startup;
   SWITCH   Iconified;
   KEY      PortName;
   KEY      IntervalTime;
   NUMBER   CpuDisplay;
   KEY      Host;
   KEY      User;
   KEY      Password;
   SWITCH   SingleWindows;
   KEY      Command;
};

#define TEMPLATE            "TOOLPRI/K/N,STARTUP/K,ICONIFIED/S,PORTNAME/K," \
                            "INTERVALTIME=IT/K,CPUDISPLAY=CPU/K/N," \
                            "HOST=H/K,USER=U/K,PASSWORD=PW/K," \
                            "SW=SINGLEWINDOWS/S," \
                            "COMMAND=C/K"

#define OPT_COUNT           sizeof(struct Args)/sizeof(LONG)


#endif /* STARTUP_DEFS_H */
