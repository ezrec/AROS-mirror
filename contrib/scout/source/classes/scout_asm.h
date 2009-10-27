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
scout_asm.s
 */

LONG ASM FAR MyCause( REG(a1, struct Interrupt *is), REG(a0, APTR data) );
APTR ASM FAR GetVBR( void );
ULONG ASM FAR GetCACR( void );
ULONG ASM FAR GetPCR( void );
void ASM FAR ReadZeroPage( REG(a0, ULONG zerobase), REG(a1, ULONG *at100), REG(a2, ULONG *at104) );
void ASM FAR InstallPatches( void );
void ASM FAR UninstallPatches( void );
void ASM FAR ClearTaskData( void );
ULONG ASM FAR GetTaskData( REG(d0, struct Task *task) );
struct Task * ASM FAR GetAddedTask( void );
void ASM FAR FreeTaskBuffer( void );

extern FAR struct Library *AsmTimerBase;
extern FAR ULONG TotalMicros1;
extern FAR ULONG TotalMicros2;
extern FAR UBYTE SwitchState;
extern FAR UBYTE PatchesInstalled;

