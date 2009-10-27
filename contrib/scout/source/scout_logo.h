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


#define USE_SCOUT_LOGO_COLORS
#define USE_SCOUT_LOGO_BODY


#ifdef USE_SCOUT_LOGO_COLORS
extern const ULONG scout_logo_colors[12];
#endif

#define SCOUT_LOGO_WIDTH        25
#define SCOUT_LOGO_HEIGHT       75
#define SCOUT_LOGO_DEPTH         2
#define SCOUT_LOGO_COMPRESSION   1
#define SCOUT_LOGO_MASKING       0

#ifdef USE_SCOUT_LOGO_HEADER
const struct BitMapHeader scout_logo_header =
{ 25,75,0,0,2,0,1,0,0,10,11,25,75 };
#endif

#ifdef USE_SCOUT_LOGO_BODY
extern const UBYTE scout_logo_body[635];
#endif
