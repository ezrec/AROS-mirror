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

#ifdef __SASC__
bigint bytes(name,total,block)
char *name;
bigint *total;
int *block;
#else
long long bytes(name,total,block)
char *name;
long long *total;
int *block;
#endif
{
    struct InfoData __aligned infodata;
    BPTR mylock,testlock;
#ifdef __SASC__
    bigint free, temp1, ret;

    free = i64_atoi( "-1" );
#else
    long long free=-1;
#endif

    if (!(mylock=Lock(name,ACCESS_READ))) {
#ifdef __SASC__
        *total = i64_atoi( "0" );
        *block=0;
        ret = i64_atoi( "0" );

        return ret;
#else
        *total=0;
        *block=0;
        return(0);
#endif
    }
    Info(mylock,&infodata);
    if (ramdisk_lock) {
        testlock=DupLock(mylock);
        testlock=getrootlock(testlock);
        if (CompareLock(testlock,ramdisk_lock)==LOCK_SAME)
#ifdef __SASC__
            free = i64_uset( AvailMem( 0 ) );
#else
          free=AvailMem(0);
#endif
        UnLock(testlock);
#ifdef __SASC__
        temp1 = i64_atoi( "-1" );

        if ( i64_cmp( free, temp1 ) == I64_GREATER )
        {
            bigint quot, rema;

            *total = free;

            temp1 = i64_set( infodata.id_BytesPerBlock );

//          *block=free/infodata.id_BytesPerBlock;
            i64_udiv( free, temp1, &quot, &rema );

            *block = quot.lo;

            UnLock( mylock );

            return free;
        }
    }

    *total = i64_uumul( infodata.id_NumBlocks, infodata.id_BytesPerBlock );
//  *total=((long long)infodata.id_BytesPerBlock)*infodata.id_NumBlocks;

    temp1 = i64_uumul( infodata.id_NumBlocksUsed, infodata.id_BytesPerBlock );

    free = i64_sub( *total, temp1 );
//  free=*total-(infodata.id_BytesPerBlock*((long long)infodata.id_NumBlocksUsed));

#else
        if (free>-1) {
            *total=free;
            *block=free/infodata.id_BytesPerBlock;
            UnLock(mylock);
            return(free);
        }
    }
    *total=((long long)infodata.id_BytesPerBlock)*infodata.id_NumBlocks;
    free=*total-(infodata.id_BytesPerBlock*((long long)infodata.id_NumBlocksUsed));
#endif
    *block=infodata.id_NumBlocks-infodata.id_NumBlocksUsed;
    UnLock(mylock);
    return(free);
}

struct TagItem obtain_tags[]={
    {OBP_Precision,PRECISION_EXACT},
    {OBP_FailIfBad,TRUE},
    {TAG_DONE,0}};

void get_colour_table()
{
    int a,pen;
    int num;
    struct ColorMap *cm;

    for (a=0;a<16;a++) {
        screen_pens[a].red=config->new_palette[(a*3)];
        screen_pens[a].green=config->new_palette[(a*3)+1];
        screen_pens[a].blue=config->new_palette[(a*3)+2];
//        screen_pens[a].pen=a;
        screen_pens[a].alloc=0;
    }

    num=1<<((config->scrdepth > 4) ? 4 : config->scrdepth);
    cm=Window->WScreen->ViewPort.ColorMap;

    for (a=0;a<num;a++) {
        pen = ObtainBestPenA(cm,
                             screen_pens[a].red,
                             screen_pens[a].green,
                             screen_pens[a].blue,
                             (MainScreen)?obtain_tags:NULL);
        if (pen == -1) screen_pens[a].pen=FindColor(cm,
                                                    screen_pens[a].red,
                                                    screen_pens[a].green,
                                                    screen_pens[a].blue,
                                                    -1);
        else
         {
          screen_pens[a].pen=pen;
          screen_pens[a].alloc=1;
         }
//D(bug("pen[%lu] = %lu\n",(int)a,(int)screen_pens[a].pen));
    }
}

void free_colour_table()
{
/*    if (system_version2>=OSVER_39)*/ {
        int a;
        struct ColorMap *cm;

        cm=Window->WScreen->ViewPort.ColorMap;

        for (a=0;a<16;a++) {
            if (screen_pens[a].alloc) {
                ReleasePen(cm,screen_pens[a].pen);
                screen_pens[a].alloc=0;
            }
        }
    }
}

void SetDrawModes(r,fg,bg,mode)
struct RastPort *r;
UBYTE fg,bg,mode;
{
//    if (system_version2>=OSVER_39) {
        SetABPenDrMd(r,
            screen_pens[fg].pen,
            screen_pens[bg].pen,
            mode);
/*    }
    else {
        SetAPen(r,screen_pens[fg].pen);
        SetBPen(r,screen_pens[bg].pen);
        SetDrMd(r,mode);
    } */
}

void do3dbox(r,x,y,w,h)
struct RastPort *r;
int x,y,w,h;
{
    Do3DBox(r,x,y,w,h,screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
}
