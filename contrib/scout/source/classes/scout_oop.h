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
 * @author Pavel Fedin <pavel.fedin@mail.ru>
 */


/* Prototypes for functions defined in
objects/scout_oop.c
 */

struct OOPClassEntry {
    struct MinNode ce_Node;
    APTR ce_Addr;
    TEXT ce_Address[ADDRESS_LENGTH];
    TEXT ce_SuperClassName[FILENAME_LENGTH];
    TEXT ce_SuperClassAddress[ADDRESS_LENGTH];
    TEXT ce_Dispatcher[ADDRESS_LENGTH];
    TEXT ce_CoerceDispatcher[ADDRESS_LENGTH];
    TEXT ce_SuperDispatcher[ADDRESS_LENGTH];
    TEXT ce_ClassName[FILENAME_LENGTH];
};

void PrintOOPClass( STRPTR );

void SendOOPClassList( STRPTR );

#define OOPWindowObject                  NewObject(OOPWinClass->mcc_Class, NULL

BOOL InitOOP(void);
void CleanupOOP(void);
APTR MakeOOPWinClass( void );
