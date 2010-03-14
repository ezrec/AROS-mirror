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

#define FILEBASE 1
#define FILENAME 1000
#define DRAWERNAME 1001
#define FILENAMEUP 1002
#define FILENAMEDOWN 1003
#define FILENAMEPOS 1004
#define OKAY 1005
#define CANCEL 1006
#define DRIVES 1007
#define PARENT 1008

#define INTERRUPT 2000

struct direntry {
    struct direntry *last,*next;
    int type,select;
    char name[FILEBUF_SIZE];
    char size[10];
};

struct FileReqData {
    int *filetype;
    struct Window *fr_Window;
    struct FileInfoBlock *finfo;
    struct direntry *firstfile,*firstdir;
    struct DOpusRemember *filekey;
    struct DOpusFileReq *freq;
    int prevsec,prevmic;
    int fileentries,fileoffset,oldfileoffset;
    int version2,flags,fh,fw,fb,width,ww,sfh,sfw,sfy,fy;
    struct PropInfo filenameprop;
    struct Image filenameimage;
    struct StringInfo filenamesinfo,drawernamesinfo;
    struct Gadget reqgads[9];
    struct IntuiText reqtext[6];
    struct NewWindow reqwin;
};

#define ERROR_CANCEL 0
#define ERROR_NOMEM -1

#define DFRB_DIRREQ 0
#define DFRB_MULTI  1
#define DFRB_SAVE   2

#define DFRF_DIRREQ (1<<DFRB_DIRREQ)
#define DFRF_MULTI  (1<<DFRB_MULTI)
#define DFRF_SAVE   (1<<DFRB_SAVE)

