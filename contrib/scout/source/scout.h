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


/* Prototypes for functions defined in
scout.c
 */

extern struct Library * MUIMasterBase;

extern struct Task * myprocess;

extern struct MsgPort * myarexxport;

extern struct MsgPort *ScoutPort;

extern STRPTR portname;

extern APTR AP_Scout;

extern APTR WI_Main;

extern APTR globalPool;

extern CONST_STRPTR decimalSeparator;
extern struct Locale *currentLocale;

ULONG scout_main( void );

extern void kprintf(const char *,...);

extern BOOL amigaOS4;
extern BOOL morphOS;

