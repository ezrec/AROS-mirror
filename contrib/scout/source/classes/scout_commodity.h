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
objects/scout_commodity.c
 */

struct PrivateCxObj {
   struct Node     mco_Node;
   TEXT           mco_Flags;
   TEXT           mco_dummy1;
   struct MinList  mco_SubList;
   APTR            mco_dummy2;
   TEXT            mco_Name[CBD_NAMELEN];
   TEXT            mco_Title[CBD_TITLELEN];
   TEXT            mco_Descr[CBD_DESCRLEN];
   struct Task *   mco_Task;
   struct MsgPort *mco_Port;
   ULONG           mco_dummy3;
   WORD            mco_dummy4;
};

struct CxEntry {
    struct MinNode cxe_Node;
    struct PrivateCxObj *cxe_Addr;
    TEXT cxe_Address[ADDRESS_LENGTH];
    TEXT cxe_Type[NODETYPE_LENGTH];
    TEXT cxe_Pri[ADDRESS_LENGTH];
    TEXT cxe_Name[CBD_NAMELEN];
    TEXT cxe_Title[CBD_TITLELEN];
    TEXT cxe_Description[CBD_DESCRLEN];
    TEXT cxe_Task[ADDRESS_LENGTH];
    TEXT cxe_Port[ADDRESS_LENGTH];
    TEXT cxe_Unique[NUMBER_LENGTH];
    UWORD cxe_FlagsInt;
    TEXT cxe_Flags[NUMBER_LENGTH];
};

struct CxSubEntry {
    struct MinNode cxse_Node;
    TEXT cxse_Address[ADDRESS_LENGTH];
    TEXT cxse_Type[NODETYPE_LENGTH];
    TEXT cxse_Pri[NUMBER_LENGTH];
    TEXT cxse_Hotkey[FILENAME_LENGTH];
};

void PrintCx( STRPTR );

void SendCxList( STRPTR );

#define CommoditiesWindowObject              NewObject(CommoditiesWinClass->mcc_Class, NULL

APTR MakeCommoditiesWinClass( void );

