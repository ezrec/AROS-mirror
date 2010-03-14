#ifndef DOPUS_MESSAGE_H
#define DOPUS_MESSAGE_H

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

#include <dopus/dopusbase.h>

struct NetworkStuff {
    char *owner;
    char *group;
    char net_protbuf[10];
};

struct Directory {
    struct Directory *last,*next;
    char name[FILEBUF_SIZE];
    int type;
#ifdef __GNUC__
    long long size;
#else
    int size;
#endif
    int subtype;
    int protection;
    char *comment;
    char *dispstr;
    char protbuf[12],datebuf[LEN_DATSTRING+12];
    int selected;
    char *description;
    int userdata,userdata2;
    struct DateStamp date;
    UWORD owner_id,group_id;
    struct NetworkStuff *network;
    char *extension;
};

#define DOPUSMSG_GETVIS         1
#define DOPUSMSG_UPDATEDRIVE    2
#define DOPUSMSG_GETNEXTFILE    3
#define DOPUSMSG_UNSELECTFILE   4
#define DOPUSMSG_GETPRINTDIR    5

struct DOpusMessage {
    struct Message msg;
    int command;
    APTR data;
};

struct VisInfo {
    char vi_fg,vi_bg;
    char vi_shine,vi_shadow;
    struct TextFont *vi_font;
    struct Screen *vi_screen;
    char vi_stringcol[2];
    char vi_activestringcol[2];
    ULONG vi_flags;
    char *vi_language;
};

#define VISF_8POINTFONT 1
#define VISF_WINDOW     2
#define VISF_BORDERS    (1<<16)

struct DOpusArgsList {
    char *single_file;
    int   file_window;
    APTR  file_list;
    APTR  last_select;
    char *file_data;
};

struct PrintDirData {
    int win;
    char titlebuf[512];
    struct Directory *entry;
    int onlysel;
    int namelen;
};

struct DirWindowPars {
    char top_name[FILEBUF_SIZE];
    int offset;
    int hoffset;
    char *reselection_list;
    ULONG reselection_size;
    UBYTE reselection_win;
    struct DirectoryWindow *reselection_dir;
};

#define CONFIG_GET_CONFIG 1
#define CONFIG_HERES_CONFIG 2
#define CONFIG_ALL_DONE 3
#define CONFIG_NEW_HOTKEY 4
#define CONFIG_HOTKEYS_CHANGE 5

struct dopusconfigmsg {
    struct Message msg;
    int command;
    char *buffer;
};

struct configconfig {
    struct Config *config;
    struct dopusfiletype *firsttype;
    struct DOpusRemember *typekey;
    struct dopusgadgetbanks *firstbank;
    int changed;
    char configname[256];
    struct Window *Window;
    struct Screen *Screen;
    struct dopushotkey *firsthotkey;
};

#endif /* DOPUS_MESSAGE_H */
