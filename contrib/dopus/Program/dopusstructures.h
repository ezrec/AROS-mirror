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

#ifndef DOPUS_STRUCTURES
#define DOPUS_STRUCTURES

struct CustEntryMessage {
    struct Message cem_Message;
    int cem_Action;
    char *cem_Data;
    int cem_UserData;
};

struct RecursiveDirectory {
    struct RecursiveDirectory *next;
    char name[FILEBUF_SIZE];
    struct DateStamp date;
    struct RecursiveDirectory *child;
};

struct Help {
    struct Help *next;
    char *lookup,*message;
};

struct ProcessStart {
    BPTR ps_NextSeg;
    UWORD ps_JMP;
    void *ps_EntryPoint;
};

struct dopusfuncpar {
    int which,stack;
    unsigned char key,qual;
    char type,pri,delay;
};

struct recpath {
    char *path;
    struct recpath *next;
};

struct dopustaskmsg {
    struct Message msg;
    int command;
    int total,value;
    int flag;
    char *data;
};

#define TASKMSGFLAG_SINGLE 0
#define TASKMSGFLAG_DOUBLE 1

struct DirectoryWindow {
    struct DirectoryWindow *next,*last;
    int number;
    int pad;
    struct Directory *firstentry;
    struct Directory *firstfile;
    struct Directory *firstdir;
    char directory[256];
    char diskname[32];
    int total;
    int filetot,dirtot,filesel,dirsel;
#ifdef __GNUC__
    long long bytestot,bytessel;
#else
    int bytestot,bytessel;
#endif
    int offset,hoffset;
    int oldoff,oldhoff;
    int hlen;
#ifdef __SASC__
    bigint disktot,diskfree;
#else
    long long disktot,diskfree;
#endif
    int diskblock;
    char custhandler[32];
    char realdevice[32];
    char flags;
    struct DateStamp dirstamp;
    char volumename[32];
    UWORD last_owner;
    UWORD last_group;
    char owner_name[32];
    char group_name[32];
    struct xadArchiveInfo *xai;
    char *arcname;
    char arcpassword[32];
};

#define DWF_ABORTED  1
#define DWF_READONLY 2
#define DWF_ARCHIVE  4

struct RLEinfo {
    unsigned char *sourceptr;
    UBYTE **destplanes;
    UWORD imagebpr,imageheight,imagedepth;
    UWORD destbpr,destheight,destdepth;
    char masking,compression;
    int offset;
};

struct args {
    char argstring[256];
    struct args *next;
};

struct function_data {
    struct args *arg_first,*arg_current,*arg_use;
    struct DOpusRemember *arg_memkey;
    struct recpath *reload_first,*reload_current,*recursive_path;
    struct DOpusRemember *reload_memkey;
    struct Directory *entry_current,*entry_first,entry_external;
    BPTR output_file;
    int rereadsource,rereaddest;
    int fileargpos,external_flag,activewin,inactivewin;
    int function_count;
    char scriptname[80],tempfile[80];
    char source_path[256],dest_path[256];
    char last_file[256];
    struct DOpusFileReq file_request;
};

struct complete_entry {
    struct complete_entry *next;
    struct complete_entry *last;
    char name[FILEBUF_SIZE];
};

struct path_complete {
    char match[FILEBUF_SIZE];
    char path[256];
    struct complete_entry *firstentry;
    struct complete_entry *currententry;
    struct DOpusRemember *memory;
};

struct RememberData {
    char copyflags;
    char deleteflags;
    char errorflags;
    char displaypos[2][16];
    char generalflags;
    char iconflags;
    char existflags;
    char sortflags;
    char dynamicflags;
    char outputcmd[80];
    char output[80];
    char scrdepth;
    char screenflags;
    int screenmode;
    int scrw;
    int scrh;
    char dirflags;
    char defaulttool[80];
    char showdelay;
    char viewbits;
    char fadetime;
    char hiddenbit;
    char showpat[40];
    char hidepat[40];
    char icontype;
    char scrclktype;
    char showfree;
    char fontsizes[NUMFONTS];
    char fontbufs[NUMFONTS][40];
    char dateformat;
    int gadgetrows;
    char displaylength[2][16];
    char windowdelta;
    char sortmethod[2];
    char separatemethod[2];
    short wbwinx,wbwiny;
    short scr_winw,scr_winh;
    char pubscreen_name[80];
    char portname[30];
};

struct ColourTable {
    ULONG red;
    ULONG green;
    ULONG blue;
    unsigned char pen;
    char alloc;
};

#include <dopus/dopusmessage.h>

#endif
