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

#include "dopus.h"

static struct RecursiveDirectory **recurse_parent_array; /* Array of parent directories */
static char *tree_buffer,*tree_path_buffer;

void dotree(win)
int win;
{
    int a,b,ret;
    struct Directory *entry;
    char *name;

    if (status_iconified ||
        dopus_curwin[win]->total==0 ||
        str_pathbuffer[win][0]==0 ||
        (dopus_curwin[win]->flags & DWF_ARCHIVE)) return;

    name=dopus_curwin[win]->directory;
    advancebuf(win,1);
    strcpy(dopus_curwin[win]->directory,name);
    strcpy(str_pathbuffer[win],name);
    checkdir(str_pathbuffer[win],&path_strgadget[win]);
    freedir(dopus_curwin[win],win);
#ifdef __SASC__
    dopus_curwin[win]->diskfree = i64_atoi( "-1" );
    dopus_curwin[win]->disktot = i64_atoi( "-1" );
    dopus_curwin[win]->diskblock = -1;
#else
    dopus_curwin[win]->diskfree=dopus_curwin[win]->disktot=dopus_curwin[win]->diskblock=-1;
#endif
    strcpy(dopus_curwin[win]->diskname,globstring[STR_DIR_TREE]);

    dostatustext(globstring[STR_SCANNING_TREE]);
    busy();
    data_active_window=win;

    ret=recursedir(str_pathbuffer[win],NULL,R_GETNAMES,0);

    if (first_recurse) {
        recurse_parent_array=LAllocRemember(&recurse_dir_key,(recurse_max_depth+1)*sizeof(APTR),MEMF_CLEAR);
        if ((tree_buffer=LAllocRemember(&recurse_dir_key,1024,MEMF_CLEAR))) {
            tree_path_buffer=LAllocRemember(&recurse_dir_key,1024,MEMF_CLEAR);
            dostatustext(globstring[STR_BUILDING_TREE]);
            ret=1;
            build_tree(first_recurse);
        }
        LFreeRemember(&recurse_dir_key);
    }

    entry=dopus_curwin[win]->firstentry;
    a=0;
    while (entry) {
        b=strlen(entry->comment);
        if (b>a) a=b;
        entry=entry->next;
    }
    a*=scrdata_font_xsize;
    a+=2;
    if (a<scrdata_dispwin_nchars[win]) a=scrdata_dispwin_nchars[win];
    dopus_curwin[win]->hlen=a;
D(bug("dotree: %ld, %ld\n",dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win]));
    refreshwindow(win,3);
D(bug("dotree: %ld, %ld\n",dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win]));
    if (ret!=-1 && ret!=-10) okay();
    unbusy();
}

int build_tree(rec)
struct RecursiveDirectory *rec;
{
    static int recdepth;
    int a,b;

    while (rec) {
        b=0;
        if (tree_path_buffer) strcpy(tree_path_buffer,str_pathbuffer[data_active_window]);
        for (a=0;a<recdepth;a++) {
            if (!recurse_parent_array || !recurse_parent_array[a])
                tree_buffer[b++]='|';
            else {
                if (recurse_parent_array[a]->next) tree_buffer[b++]='|';
                else tree_buffer[b++]=' ';
                if (tree_path_buffer)
                    TackOn(tree_path_buffer,recurse_parent_array[a]->name,1024);
            }
            tree_buffer[b++]=' '; tree_buffer[b++]=' ';
        }
        if (rec->next) strcpy(&tree_buffer[b],"|--");
        else strcpy(&tree_buffer[b],"+--");
        strcpy(&tree_buffer[b+3],rec->name);
        if (tree_path_buffer) TackOn(tree_path_buffer,rec->name,1024);
        if (!(addfile(dopus_curwin[data_active_window],data_active_window,
            rec->name,0,ENTRY_CUSTOM,&rec->date,tree_buffer,0,
            CUSTOMENTRY_DIRTREE,FALSE,tree_path_buffer,NULL,0,0))) return(0);
        if (rec->child) {
            recurse_parent_array[recdepth]=rec;
            recdepth++;
            if (!(build_tree(rec->child))) return(0);
            recdepth--;
            recurse_parent_array[recdepth]=NULL;
        }
        rec=rec->next;
    }
    return(1);
}

void draw_dirtree_gfx(rp,x,y,type)
struct RastPort *rp;
int x,y,type;
{
    int half_xsize,half_ysize,x_plus_half;

    half_xsize=scrdata_font_xsize/2;
    half_ysize=scrdata_font_ysize/2;
    x_plus_half=x+half_xsize;
    if (status_flags&STATUS_SQUAREPIXEL) ++x_plus_half;

    switch (type) {
        case DIRTREEGFX_VERT:          /* |  */
        case DIRTREEGFX_VERTCROSS:     /* |­ */
        case DIRTREEGFX_BRANCH:        /* ¹­ */
            rectfill(rp,x_plus_half,y,(status_flags&STATUS_SQUAREPIXEL)?1:2,(type==DIRTREEGFX_BRANCH)?half_ysize:scrdata_font_ysize);
            if (type!=DIRTREEGFX_VERT)
             {
              Move(rp,x_plus_half,y+half_ysize);
              Draw(rp,x+scrdata_font_xsize-1,y+half_ysize);
             }
            break;
        case DIRTREEGFX_CROSS:         /* ­  */
        case DIRTREEGFX_FINALCROSS:    /* -  */
            Move(rp,x,y+half_ysize);
            Draw(rp,x+scrdata_font_xsize-((type==DIRTREEGFX_CROSS)?1:2),y+half_ysize);
            break;
    }
}
