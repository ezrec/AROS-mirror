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

struct NetworkStuff {
	char *owner;
	char *group;
	char net_protbuf[10];
};

struct Directory {
	struct Directory *last,*next;
	char name[32];
	int type;
	int size;
	int subtype;
	int protection;
	char *comment;
	char *dispstr;
	char protbuf[12],datebuf[20];
	int selected;
	char *description;
	int userdata,userdata2;
	struct DateStamp date;
	UWORD owner_id,group_id;
	struct NetworkStuff *network;
};

struct RecursiveDirectory {
	struct RecursiveDirectory *next;
	char name[32];
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

struct dopusfunction {
	char name[16];
	int which,stack;
	unsigned char key,qual;
	char type,pri,delay;
	char fpen,bpen;
	char pad;
	char *function;
};

struct newdopusfunction {
	char *name;
	int pad3[3];
	int which,stack;
	unsigned char key,qual;
	char type,pri,delay;
	char fpen,bpen;
	char pad;
	char *function;
};

struct dopusfiletype {
	struct dopusfiletype *next;
	char type[32];
	char typeid[8];
	char actionstring[FILETYPE_FUNCNUM][40];
	int which[FILETYPE_FUNCNUM],stack[FILETYPE_FUNCNUM];
	char pri[FILETYPE_FUNCNUM],delay[FILETYPE_FUNCNUM];
	unsigned char *recognition;
	char *function[FILETYPE_FUNCNUM];
	char *iconpath;
};

struct dopusgadgetbanks {
	struct newdopusfunction gadgets[GADCOUNT];
	struct dopusgadgetbanks *next;
};

struct dopushotkey {
	struct dopushotkey *next;
	UWORD code,qualifier;
	char name[40];
	struct dopusfunction func;
};

struct Config {
	USHORT version;
	USHORT magic;

	char copyflags,deleteflags,errorflags,generalflags,iconflags;
	char existflags,sepflags,sortflags,dynamicflags;
	char sortmethod[2];

	char hotkeyflags;

	char menutit[5][16];
	struct newdopusfunction menu[MENUCOUNT];

	struct dopusfunction drive[DRIVECOUNT];

	char outputcmd[80],output[80];
	int gadgetrows;

	char separatemethod[2];

	char language[30];

	char displaypos[2][16];
	char displaylength[2][16];

	char pubscreen_name[80];

	USHORT Palette[16];
	char gadgettopcol,gadgetbotcol;
	char statusfg,statusbg;
	char filesfg,filesbg,filesselfg,filesselbg;
	char dirsfg,dirsbg,dirsselfg,dirsselbg;
	char clockfg,clockbg;
	char requestfg,requestbg;
	char disknamefg,disknamebg,disknameselfg,disknameselbg;
	char slidercol,arrowfg,arrowbg,littlegadfg,littlegadbg;

	char pad3;

	char scrdepth;
	char screenflags;
	int screenmode;
	int scrw,scrh;
	char fontbuf[40];
	char arrowpos[3];

	char pad4;

	char startupscript[80];
	char dirflags;
	unsigned char bufcount;

	char pad5[2];

	char autodirs[2][30];
	char pad5a[80];
	UWORD hotkeycode,hotkeyqual;

	char toolicon[80],projecticon[80],drawericon[80],defaulttool[80];
	char priority;
	char showdelay,viewbits,fadetime,tabsize;

	char pad7[2];

	char hiddenbit;
	char showpat[40],hidepat[40];
	char showpatparsed[40],hidepatparsed[40];
	char icontype,scrclktype,showfree;

	char pad8;

	short iconx,icony;
	short wbwinx,wbwiny;

	char configreturnscript[80];

	char fontsizes[NUMFONTS];
	char fontbufs[NUMFONTS][40];

	char uniconscript[80];
	char sliderbgcol;

	char pad_foo;

	short scr_winx,scr_winy;
	short scr_winw,scr_winh;

	char morepadding[231];

	char old_displaypos[2][8];
	char dateformat,addiconflags;
	char stringfgcol,stringbgcol;
	char namelength[2];
	char sliderwidth,sliderheight;
	char formatflags;
	short iconbutx,iconbuty;
	char stringheight;
	char stringselfgcol,stringselbgcol;
	char generalscreenflags;

	struct Rectangle scrollborders[2];

	char old_displaylength[2][8];

	char shellstartup[30];

	char windowdelta;

	char pad9a[397];

	int loadexternal;

	ULONG new_palette[48];

	char arrowsize[3];

	char slider_pos;

	short config_x;
	short config_y;

	char pad10[1414];
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

struct ConfigStuff {
	struct Config *config;
	struct DOpusRemember *typekey;
	struct dopusfiletype *firsttype;
	struct dopusgadgetbanks *firstbank,*curbank;
	struct dopushotkey *firsthotkey;
};

struct DirectoryWindow {
	struct DirectoryWindow *next,*last;
	int number,type;
	struct Directory *firstentry;
	struct Directory *firstfile;
	struct Directory *firstdir;
	char directory[256];
	char diskname[32];
	int total;
	int filetot,dirtot,bytestot;
	int filesel,dirsel,bytessel;
	int offset,hoffset;
	int oldoff,oldhoff;
	int hlen;
	int disktot,diskfree,diskblock;
	char custhandler[32];
	char realdevice[32];
	char flags;
	struct DateStamp dirstamp;
	char volumename[32];
	UWORD last_owner;
	UWORD last_group;
	char owner_name[32];
	char group_name[32];
};

#define DWF_ABORTED  1
#define DWF_READONLY 2

struct RLEinfo {
	unsigned char *sourceptr;
	UBYTE **destplanes;
	USHORT imagebpr,imageheight,imagedepth;
	USHORT destbpr,destheight,destdepth;
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
	char name[32];
};

struct path_complete {
	char match[32];
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
	char pen;
	char alloc;
};

#include "dopusmessage.h"

#endif
