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

void lsprintf __ARGS((char *,char *,...));

void main(int,char **);
void get_vis_info(struct VisInfo *,char *);
dopus_message(int,APTR,char *);
void fill_out_req(struct RequesterBase *,struct VisInfo *);
struct Gadget *addreqgadgets(struct RequesterBase *,struct TagItem **);
void fix_listview(struct RequesterBase *,struct DOpusListView *);
void setup_strings(void);

doiconinfo(struct VisInfo *,char *);

void drawicon(struct Window *,struct Image *,struct Rectangle *);
void drawrecaround(struct RastPort *,int,int,int,int,int,int,int,int);
void compicon(struct RastPort *,struct Image *,struct Rectangle *);
void remapimage(struct Image *);
void offttgads(struct Window *,struct Gadget *);
void onttgads(struct Window *,struct Gadget *);

doroot(char *);
getroot(char *,struct DateStamp *);
