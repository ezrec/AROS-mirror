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
scout_net.c
 */

extern BOOL clientstate;

extern BOOL shellstate;

extern char username[11];

extern char password[129];

void failtcp( void );

int inittcp( void );

int sgets(int , STRPTR , int );

void PrintNetHelp( STRPTR );

int VARARGS68K STDARGS SendDaemon( CONST_STRPTR , ... );

BOOL ReceiveDecodedEntry( void *structure , ULONG length );

int ConnectToServer( void );

ULONG netshellclient( void );

int isNetCall( void );

int isCommand( STRPTR , CONST_STRPTR );

int VARARGS68K STDARGS SendClient( CONST_STRPTR , ... );

BOOL SendEncodedEntry( void *structure, ULONG length );

long VARARGS68K STDARGS SendResultString( CONST_STRPTR , ... );

void VARARGS68K STDARGS PrintFOneLine( BPTR hd, CONST_STRPTR fmt, ... );

ULONG ExecuteCommand( STRPTR );

ULONG netdaemon(void);

ULONG VARARGS68K STDARGS MyDoCommand( CONST_STRPTR , ... );

extern CONST_STRPTR CMD_END;

