/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#define DECR_COL0 0L
#define DECR_COL1 1L
#define DECR_POINTER 2L
#define DECR_SCROLL 3L
#define DECR_NONE 4L
#define PP_OPENERR -1L
#define PP_READERR -2L
#define PP_NOMEMORY -3L
#define PP_CRYPTED -4L
#define PP_PASSERR -5L
#define PP_UNKNOWNPP -6L

ULONG ppLoadData(char *,ULONG,ULONG,char **,int *,BOOL(*)());

#warning Have a look at this later
/* AROS: We don't support pragmas 

#pragma libcall PPBase ppLoadData 1E BA910806
*/
