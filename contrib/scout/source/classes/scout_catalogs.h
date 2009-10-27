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
objects/scout_catalogs.c
 */

struct CatalogEntry {
    struct MinNode ce_Node;
    struct Catalog *ce_Addr;
    TEXT ce_Address[ADDRESS_LENGTH];
    TEXT ce_Name[NODENAME_LENGTH];
    TEXT ce_Version[VERSION_LENGTH];
    TEXT ce_Language[TEXT_LENGTH];
};

void PrintCatalogs( STRPTR );

void SendCatalogList( STRPTR );

#define CatalogsWindowObject                 NewObject(CatalogsWinClass->mcc_Class, NULL

APTR MakeCatalogsWinClass( void );

#if !defined(__SASC)
#pragma pack(2)
#endif

struct IntLocaleBase
{
    struct LocaleBase        lb_LocaleBase;
    struct SignalSemaphore   lb_LocaleLock;
    struct SignalSemaphore   lb_CatalogLock;
    struct MinList           lb_CatalogList;
};

#if !defined(__SASC)
#pragma pack()
#endif
