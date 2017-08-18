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

struct PrintHandle;

#if defined(__AROS__)
#include <stdio.h>
#define lsprintf sprintf
#endif
//void lsprintf __ARGS((char *,char *,...));

void get_vis_info(struct VisInfo *,char *);
int dopus_message(int,APTR,char *);
void fill_out_req(struct RequesterBase *,struct VisInfo *);
struct Gadget *addreqgadgets(struct RequesterBase *,struct TagItem **,struct Gadget **);
void fix_req_gadgets(void);
void border_text(struct RequesterBase *,Object_Border *,char *);

void set_print_env(PrintData *);
void get_print_env(PrintData *);

void activate_next_gadget(struct Gadget *,struct Window *);
int get_file_byrequest(struct Gadget *,struct Window *,int);
int check_error(struct RequesterBase *,char *,int);

void do_print(struct VisInfo *,char *,struct DOpusArgsList *,int,char **);
void get_print_default(PrintData *);
void show_headerfooter_gadgets(struct Gadget **,struct Window *,PrintData *,int);
void read_headerfooter_gadgets(struct Gadget **,struct Window *,PrintData *,int);
void print_status(struct PrintHandle *,char *,int);

int printfile(struct RequesterBase *,char *,PrintData *,struct Requester *);
int check_print_abort(struct RequesterBase *);

int check_print_gadget(struct Window *,struct Gadget *,int,int,UWORD);

void do_printdir(struct VisInfo *,char *,int);
void printdirectory(struct RequesterBase *,char *,int,char *,int);

void save_printdir_env(int,int,char *);
void read_printdir_env(int *,int *,char *);
