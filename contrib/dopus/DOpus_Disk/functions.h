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

int main(int,char **);
void get_vis_info(struct VisInfo *,char *);
int dopus_message(int,APTR,char *);
void fill_out_req(struct RequesterBase *,struct VisInfo *);
struct Gadget *addreqgadgets(struct RequesterBase *,struct TagItem **,int,int *);
int check_error(struct RequesterBase *,char *,int);
int check_disk(struct RequesterBase *,struct IOExtTD *,char *,int);
int check_abort(struct Window *window);
int check_blank_disk(struct RequesterBase *,char *,char *);
void set_env(char *,struct Gadget *,int,struct DOpusListView *);
void get_env(char *,struct Gadget *,int,struct DOpusListView *);
void fix_listview(struct RequesterBase *,struct DOpusListView *);
void select_device(struct DOpusListView *,char *);
int like_devices(struct DeviceNode *,struct DeviceNode *);
void drive_motor(struct IOExtTD *,int);
void show_sel_item(struct DOpusListView *);

int open_device(char *,struct DeviceHandle *);
void close_device(struct DeviceHandle *);

char **get_device_list(struct DOpusRemember **,char *);
void sort_device_list(char **);
struct DeviceNode *find_device(char *);
void inhibit_drive(char *,ULONG);
ULONG do_checksum(ULONG *);
int do_writeblock(struct IOExtTD *,APTR,ULONG);
void border_text(struct RequesterBase *,Object_Border *,char *);

void diskop_format(struct VisInfo *,char *,int,char **);
void show_device_info(struct RequesterBase *,Object_Border *,char *);
void getsizestring(char *,ULONG);
void getfloatstr(double,char *);
int do_format(struct RequesterBase *,Object_Border *,char *,char *,ULONG,char);
int do_initialise(struct IOExtTD *,char *,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);
int do_raw_format(struct RequesterBase *,Object_Border *,struct IOExtTD *,ULONG,ULONG,ULONG,ULONG,ULONG);
void write_trashcan(struct RequesterBase *,Object_Border *,char *);

void diskop_diskcopy(struct VisInfo *,char *,int,char **);
void show_diskcopy_info(struct RequesterBase *,Object_Border *,char *);
int do_diskcopy(struct RequesterBase *,Object_Border *,char *,char *,int,int,int);
void bump_disk_name(char *);

void diskop_install(struct VisInfo *,int,char **);
void show_install_info(struct RequesterBase *,Object_Border *,char *);
int install_compare_block(ULONG *,ULONG *,ULONG);
int do_install(struct RequesterBase *,Object_Border *,char *,int,int);
