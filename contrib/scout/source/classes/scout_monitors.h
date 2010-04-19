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
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

struct MonitorEntry {
    struct MinNode mon_Node;
    APTR mon_Addr;
    TEXT mon_Address[ADDRESS_LENGTH];
    TEXT mon_Name[NODENAME_LENGTH];
    TEXT mon_Type[NODETYPE_LENGTH];
    TEXT mon_Pri[NUMBER_LENGTH];
    TEXT mon_Subsystem[NODESUBSYS_LENGTH];
    TEXT mon_Subtype[NODESUBTYPE_LENGTH];
};

void PrintMonitors( STRPTR );

void SendMonitorsList( STRPTR );

APTR MakeMonitorsWinClass( void );
