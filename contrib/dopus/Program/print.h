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

#define PSTYLE_NORMAL  1
#define PSTYLE_NLQ     2
#define PSTYLE_BOLD    3
#define PSTYLE_ITALICS 4
#define PSTYLE_ULINE   5
#define PSTYLE_DSTRIKE 6
#define PSTYLE_SHADOW  7

#define HEADER_OFF     0
#define HEADER_TITLE   1
#define HEADER_DATE    2
#define HEADER_PAGE    4

struct PrintData {
	int length;
	int lmargin,rmargin;
	int tabs;
	int style;
	int formfeed;
	int header,footer;
	int hstyle,fstyle;
	char title[80];
};

extern struct PrintData *printdata;
